#include "break_even_volatility.hpp"
#include <stdio.h> // include the FILENAME_MAX
#include <functional>
#include <numeric>
#include <fstream>
#include <stdlib.h> // I THINK IT IS FOR GNUPLOT
// #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif

std::string get_dir();
std::vector<double> linspace(double a, double b, size_t n);
const std::vector<double>& PnL_Hedged(const option& opt);
double breakeven_vol(option& opt, const double& tol, double up_vol, double low_vol);

int main(int argc, char* argv[])
{
    // enter the file name of underlying and interest rate data
    std::string underlying_filename("");
    std::string interestrate_filename(""); /* comment this line for constant rate */
    
    // read underlying and interest rate data
    std::string current_dir = get_dir();
    current_dir.erase(current_dir.size() - 5); // remove "build" from the directory
    time_series underlying(current_dir + underlying_filename, "S&P500");
    time_series interestrate(current_dir + interestrate_filename, "TBill"); /* comment this line for constant rate */
    // double rate = 0.01; /* uncomment this line for constant rate */
    
    // create a vector of strike level for creating the volatility smile
    double low_strike = 0.2;
    double up_strike = 1.8;
    std::vector<double> strike = linspace(low_strike, up_strike, (up_strike - low_strike) * 1000); // every 0.1% strike level
    
    // target term
    size_t term = 365;
    
    // target date
    std::string target_date = "18/12/2017";
    
    // vol initial bound
    double up_vol = 0.01;
    double low_vol = 2.;
    double mid_vol = (up_vol + low_vol) / 2.;
    
    // tolerance
    double tol = 0.0001;
    
    // create a vector to store the resulting fair vols
    std::vector<double> fair_vol(strike.size());
    
    option target_option(underlying, strike[0], mid_vol, interestrate, target_date, term, 1);
    
    // transform the strike vector from percentage to price level
    double S0 = target_option.get_underlying_data()[0];
    std::transform(strike.begin(), strike.end(), strike.begin(), [S0](double& arg){return arg * S0;});

    for(size_t i = 0; i < strike.size(); i++)
    {
        target_option.modify_strike(strike[i]);
        fair_vol[i] = breakeven_vol(target_option, tol, up_vol, low_vol);
    }
    
    /*/ graph the resulting volatility smile

    std::ofstream f("break_vol.dat");
    f << strike << "\t" << fair_vol << std::endl;
    f.close();
    std::system("gnuplot break_vol.dat");*/ //to test

    return 0;
}

// function to get current directory
std::string get_dir()
{
    char buff[FILENAME_MAX];
    get_current_dir(buff, FILENAME_MAX);
    std::string current_dir(buff);
    return current_dir;
}

// function to generate a equally spaced vector with bound [a, b] and size n+1
std::vector<double> linspace(double a, double b, size_t n)
{
    std::vector<double> result(n + 1);
    double step = (b - a) / n;
    
    for(size_t i = 0; i <= n ; i++)
    {
        result[i] = a + i * step;
    }
    
    return result;
}

// function to calculate the daily PNL of a delta hedged option position
const std::vector<double>& PnL_Hedged(const option& opt, const double& N)
{
	std::vector<double> price = opt.BS_price();
        std::vector<double> delta = opt.BS_delta();
	std::vector<double> PnL_opt(price.size()-1);
	std::vector<double> PnL_hedge(price.size()-1);
	std::vector<double> underlying_data = opt.get_underlying_data();
	
	for (size_t i = 0; i < underlying_data.size() - 1; i++)
	{
		PnL_opt[i] = N*(price[i+1] - price [i]);
		PnL_hedge[i] = N*delta[i]*(underlying_data[i+1] - underlying_data[i]);
	}
    
    return std::transform(PnL_opt.begin(), PnL_opt.end(), PnL_hedge.begin(), PnL_opt.begin(), std::minus<double>());
}

// function to get the breakeven vol which makes the delta hedged PNL of the option = 0
double breakeven_vol(option& opt, const double& tol, double up_vol, double low_vol)
{
    double mid_vol = (up_vol + low_vol) / 2.;
    opt.modify_vol(mid_vol);
    
    // compute PNL with initial mid vol
    std::vector<double> PnL = PnL_Hedged(opt);
    double acc_PnL = std::accumulate(PnL.begin(), PnL.end(), 0);
    
    double up_acc_PnL;
    double low_acc_PnL;
    
    if(std::abs(acc_PnL) > tol)
    {
        // compute PNL with upper vol
        opt.modify_vol(up_vol);
        PnL = PnL_Hedged(opt);
        up_acc_PnL = std::accumulate(PnL.begin(), PnL.end(), 0);
        
        // compute PNL with lower vol
        opt.modify_vol(low_vol);
        PnL = PnL_Hedged(opt);
        low_acc_PnL = std::accumulate(PnL.begin(), PnL.end(), 0);
        
        // account for the case where PNLs from upper vol and lower vol have the same sign
        if(up_acc_PnL * low_acc_PnL > 0)
        {
            std::cout << "No solution can be found." << std::endl;
            mid_vol = 0;
        }
        else
        {
            while(std::abs(acc_PnL) > tol)
            {
                if(acc_PnL * up_acc_PnL > 0)
                {
                    // if PNL with mid vol has the same sign as the PNL with upper vol, replace upper vol with current mid vol
                    up_vol = mid_vol;
                    up_acc_PnL = acc_PnL; // replace the PNL with upper vol by the PNL with current mid vol
                }
                else
                {
                    // if PNL with mid vol has the same sign as the PNL with lower vol, replace lower vol with current mid vol
                    low_vol = mid_vol;
                    low_acc_PnL = acc_PnL; // replace the PNL with upper vol by the PNL with current mid vol
                }
                // compute PNL with new mid vol (midpoint of the new upper vol and lower vol)
                mid_vol = low_vol + ((up_vol - low_vol)/2.);
                opt.modify_vol(mid_vol);
                const std::vector<double> PnL = PnL_Hedged(opt);
                double acc_PnL = std::accumulate(PnL.begin(), PnL.end(), 0);
            }
        }
    }
    return mid_vol;
}
