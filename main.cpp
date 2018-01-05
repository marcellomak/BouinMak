#include "break_even_volatility.hpp"
#include <stdio.h>
// #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define get_current_dir _getcwd
#else
#include <unistd.h>
#define get_current_dir getcwd
#endif

const std::vector<double> PnL_Hedged(const option& opt)
{
	double price = opt.BS_price();
	double delta = opt.BS_delta();
	return std::transform(price.begin(),price.end(), delta.begin(), price.begin(), std::minus<double>());
}

const std::vector<double> Fair_vol(const option& opt, const double& tol)
{
	const std::vector<double> PnL = PnL_Hedged(opt);
	double res = std::accumulate(PnL.begin(), PnL.end(),0);
	
	double up_vol = opt.get_volatility() + 20./100.;
	double low_vol = 0.;
	
	while (res>tol)
	{
		double mid_vol = (up_vol + low_vol)/2.;
		opt.modify_vol(mid_vol);
		PnL = PnL_Hedged(opt);
	        res = std::accumulate(PnL.begin(), PnL.end(),0);
		if res > 0
		{
			up_vol = mid_vol;
		}
		else if res < 0
		{
			low_vol = mid_vol;
		}
	}

	return res;
}

std:string get_dir();
std::vector<double> linspace(double a, double b, size_t n);

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
    std::vector<double> strike = linspace(low_strike, up_strike, (up_strike - low_strike) * 1000) // every 0.1% strike level
    
    // target term
    size_t term = 365;
    
    // target date
    target_date = "18/12/2017"
    
    // vol initial bound
    double up_col = 0.01;
    double low_vol = 2.;
    double mid_vol = (up_vol + low_vol) / 2.;
    
    option target_option(underlying, strike[0], mid_vol, interestrate, target_date, term, 1);
    
    
    return 0;
    /*
    std::cout << "TEST Class time series" << std::endl;
    time_series hihi("/Users/Marcello/Documents/My Documents/Academic/2016 Master/Master 203/S3/C++/Test/data.csv", "EuroStoxx 50");
    std::cout << hihi.get_dataname() << std::endl;
    std::vector<time_t> datadate = hihi.get_date();
    std::cout << datadate[998] << std::endl;
    
    time_t targett = c_str_timet("18/12/2017");
    std::cout << targett << std::endl;
    
    std::vector<ptrdiff_t> temp = hihi.get_datapos("18/12/2017", 30);
    
    std::cout << temp[0] << " and " << temp[1] <<std::endl;
    std::vector<double> data = hihi.get_data(temp[0], temp[1]);
    std::cout << data[0] << " and " << data[data.size() - 1] << std::endl;
    
    std::cout << std::endl;
    std::cout << "TEST Class call option" << std::endl;
    
    time_series underlying("/Users/Marcello/Documents/My Documents/Academic/2016 Master/Master 203/S3/C++/Project/optiontest.csv", "testunderlying");
    time_series rate("/Users/Marcello/Documents/My Documents/Academic/2016 Master/Master 203/S3/C++/Project/ratetest.csv", "testrate");
    
    call_option option1(underlying, 100., 0.2, rate, "18/12/2017", 30);
    std::vector<double> optionprice = option1.BS_price();
    std::vector<double> optiondelta = option1.BS_delta();
    
    for(int i = 0; i < optionprice.size(); i++)
    {
        std::cout << optionprice[i] << " and " << optiondelta[i] << std::endl;
    }
    */
}

// a function to get current directory
std:string get_dir()
{
    char buff[FILENAME_MAX];
    get_current_dir(buff, FILENAME_MAX);
    std::string current_dir(buff);
    return current_dir;
}

std::vector<double> linspace(double a, double b, size_t n)
{
    std::vector<double> result(n + 1);
    double step = (b - a) / n;
    
    for(int i = 0; i <= n ; i++)
    {
        result[i] = a + i * step;
    }
    
    return result;
}

/*
EuroStoxx 50
1513551600
1513551600
978 and 998
5017.45 and 5242.28
*/
