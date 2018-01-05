#include <iostream>
#include <vector>
#include <numeric>
#include "break_even_volatility.hpp"


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

int main(int argc, char* argv[])
{
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

    return 0;
}

/*
EuroStoxx 50
1513551600
1513551600
978 and 998
5017.45 and 5242.28
*/
