#include "break_even_volatility.hpp"
#include <stdio.h> // include the FILENAME_MAX
#include <functional>
#include <numeric>
//#include "C:\Users\MY\BouinMak\CImg.h"
#define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif

std::string get_dir();
std::vector<double> linspace(double a, double b, size_t n);
std::vector<double> PnL_Hedged(const option& opt, double N, bool BSR);
double breakeven_vol(option opt, const double& tol, double up_vol, double low_vol, bool BSR);

int main(int argc, char* argv[])
{
    /*******************************************
                      *INPUT*
    ********************************************/
    
    // DATA - enter the file name of underlying and interest rate data (enter the constant interest rate)
    std::string underlying_filename("S&P500.csv");
    //std::string interestrate_filename("LIBOR.csv"); /* comment this line and line 56 for constant rate */
    double interestrate = 0.0; /* uncomment this line for constant rate */
    
    // TARGET DATE AND TERM OF THE IMPLIED VOLATILITY
    std::string target_date = "12/12/2017";
    size_t term = 365;
    
    // TARGET STRIKE BOUNDARY AND NUMBER OF STEPS
    double up_strike = 1.2;
    double low_strike = 0.8;
    size_t N = 400;
    
    // INITIAL VOLATILITY BOUND LEVELS
    double up_vol = 0.5;
    double low_vol = 0.002;
    
    // TOLERANCE LEVEL - PNL
    double tol = 0.000001;
    
    // OUTPUT FILE NAME
    std::string output_filename("BreakevenVolOutput.csv");

    /*****************************************/
    
    // read underlying and interest rate data
    std::string current_dir = get_dir();
    current_dir.erase(current_dir.size() - 5); // remove "build" from the directory
    time_series underlying(current_dir + underlying_filename, "S&P500");
    //time_series interestrate(current_dir + interestrate_filename, "LIBOR"); /* comment this line for constant rate */
    
    // create a vector of strike level for creating the volatility smile
    std::vector<double> strike = linspace(low_strike, up_strike, N);
    
    // initial mid vol
    double mid_vol = low_vol + ((up_vol - low_vol)/2.);
    
    // create a vector to store the resulting fair vols
    std::vector<double> fair_vol(strike.size());
    std::vector<double> fair_vol_BSR(strike.size());
    
    double S0;
    
    try
    {
        option target_option(underlying, strike[0], mid_vol, interestrate, target_date, term, 1);
        
        // transform the strike vector from percentage to price level
        S0 = target_option.get_underlying_data()[0];
        std::transform(strike.begin(), strike.end(), strike.begin(), [S0](double& arg){return arg * S0;});
        
        // breakeven volatility based on 0 delta hedging PNL
        for(size_t i = 0; i < strike.size(); i++)
        {
            target_option.modify_strike(strike[i]);
            std::cout << "OPTION strike " << strike[i] << "; % of S0: " << strike[i] / S0 * 100 << std::endl;
            fair_vol[i] = breakeven_vol(target_option, tol, up_vol, low_vol, false);
            std::cout << "Fair vol is " << fair_vol[i] << std::endl;
            // based on Black-Scholes Robustness formula
            fair_vol_BSR[i] = breakeven_vol(target_option, tol, up_vol, low_vol, true);
            std::cout << "Fair vol (BSR) is " << fair_vol_BSR[i] << std::endl;
            std::cout << std::endl;
        }
    } catch(const char* msg) {
        std::cerr << msg << std::endl;
    }
    
    // generate output file
    std::ofstream output_file;
    output_file.open(current_dir + output_filename);
    
    output_file << "Strike Level" << "," << "Strike % of S0" << "," << "Fair Vol" << "," << "Fair Vol (BSR)" << std::endl;
    
    for(size_t i = 0; i < strike.size(); i++)
    {
        output_file << strike[i] << "," << strike[i] / S0 << "," << fair_vol[i] << "," << fair_vol_BSR[i] << std::endl;
    }
    
    output_file.close();
    
    /*
    // graph the resulting volatility smile
    cimg_library::CImg<>(strike,fair_vol).display_graph("This",1);
    */
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
std::vector<double> PnL_Hedged(const option& opt, double N, bool BSR)
{
    std::vector<double> underlying = opt.get_underlying_data();
    
	if(BSR == false)
    {
        std::vector<double> price = opt.BS_price();
        std::vector<double> delta = opt.BS_delta();
        std::vector<double> PnL_opt(price.size()-1);
        std::vector<double> PnL_hedge(price.size()-1);
        
        for(size_t i = 0; i < underlying.size() - 1; i++)
        {
            PnL_opt[i] = N*(price[i+1] - price [i]);
            PnL_hedge[i] = N*delta[i]*(underlying[i+1] - underlying[i]);
        }
        
        std::transform(PnL_opt.begin(), PnL_opt.end(), PnL_hedge.begin(), PnL_opt.begin(), std::minus<double>());
        return PnL_opt;
    }
    else
    {
        // calculation based on Black-Scholes Robustness formula
        std::vector<double> gamma = opt.BS_gamma();
        double vol = opt.get_volatility();
        double deltat = 1./252.;
        std::vector<double> PnL(underlying.size());
        double deltaS;
        
        for(size_t i = 0; i < underlying.size() - 1; i++)
        {
            deltaS = underlying[i+1] - underlying[i];
            PnL[i] = N * (0.5 * gamma[i] * pow(underlying[i], 2.) * (pow(deltaS / underlying[i], 2.) - pow(vol, 2.) * deltat));
        }
        return PnL;
    }
}

// function to get the breakeven vol which makes the delta hedged PNL of the option = 0
double breakeven_vol(option opt, const double& tol, double up_vol, double low_vol, bool BSR)
{
    double mid_vol = low_vol + ((up_vol - low_vol)/2.);
    opt.modify_vol(mid_vol);
    
    // compute PNL with initial mid vol
    std::vector<double> PnL = PnL_Hedged(opt, 1., BSR);
    
    double acc_PnL;
    acc_PnL = 0.;
    std::for_each(PnL.begin(), PnL.end(), [&acc_PnL](double arg){acc_PnL += arg;});
    
    double up_acc_PnL;
    double low_acc_PnL;
    
    if(std::fabs(acc_PnL) > tol)
    {
        // compute PNL with upper vol
        opt.modify_vol(up_vol);
        PnL = PnL_Hedged(opt, 1., BSR);
        up_acc_PnL = 0.;
        std::for_each(PnL.begin(), PnL.end(), [&up_acc_PnL](double arg){up_acc_PnL += arg;});
        
        // compute PNL with lower vol
        opt.modify_vol(low_vol);
        PnL = PnL_Hedged(opt, 1., BSR);
        low_acc_PnL = 0.;
        std::for_each(PnL.begin(), PnL.end(), [&low_acc_PnL](double arg){low_acc_PnL += arg;});
        
        // account for the case where PNLs from upper vol and lower vol have the same sign
        if(up_acc_PnL * low_acc_PnL > 0)
        {
            std::cout << "No solution can be found." << std::endl;
            mid_vol = nan("1");
        }
        else
        {
            int iterator = 0;
            while(std::fabs(acc_PnL) > tol)
            {
                iterator = iterator + 1;
                std::cout << "#iteration " << iterator;
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
                PnL = PnL_Hedged(opt, 1., BSR);
                acc_PnL = 0.;
                std::for_each(PnL.begin(), PnL.end(), [&acc_PnL](double arg){acc_PnL += arg;});
                std::cout << "  --  Vol: " << mid_vol << " / PNL: " << acc_PnL << std::endl;
            }
        }
    }
    std::cout << "Resulting PNL: " << acc_PnL << std::endl;
    return mid_vol;
}
