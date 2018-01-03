#ifndef BREAK_EVEN_VOLATILITY_HPP
#define BREAK_EVEN_VOLATILITY_CPP
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric> //for std::iota

class time_series
{
public:
    //constructor and destructor
    time_series(const std::string& filepath, const std::string& dataname);
    ~time_series();
    
    //methods
    std::vector<std::string> get_dataname() const; // get private data member m_dataname
    std::vector<double> get_data() const; // get the whole series of data
    std::vector<double> get_data(const std::string& maturity, const size_t& day_number) const; // get a part of the series with the end date and target number of days
    std::vector<size_t> day_to_maturity(const std::string& maturity, const size_t& day_number) const; // create a vector containing daily days to maturity
    //std::vector<std::string> get_date(const std::string& maturity, const size_t& data_length) const;
    
private:
    std::string m_filepath;
    std::string m_dataname;
    std::vector<std::string> m_datadate;
    std::vector<double> m_data;
};

class vol // maybe to delete
{
    public:
        explicit vol(const size_t mat, const double strike, const std::string underlying);
        virtual ~vol();

    private:
    protected:

};
/*
class pricer
{
public:
    //constructor and destructor
    pricer(const time_series& underlying, const double& strike, const double& vol, const time_series& rate, const size_t& maturity);
    ~pricer();

    //daily price and delta of the option
    std::vector<double> BS_price() const;
    std::vector<double> BS_delta() const;

private:
    // don't know just put it here but maybe to be removed
    time_series m_underlying;
    double m_strike;
    double m_vol;
    time_series m_rate;
    size_t m_maturity;
protected:
};

class portfolio
{
public:
private:
}
*/
/*
class PnL : public vol //maybe to change
{
    public:

        //constructor and destructor
        PnL(const std::vector<double>& underlying, const double& strike, const double& vol, const std::vector<double>& rate ,const size_t& mat);
        ~PnL();

        //computation of the PnL, use of the pricers formulas
        const Hedged_PnL(const std::vector<double>& underlying, const double& strike, const double& vol, const std::vector<double>& rate, const size_t& mat) const;
        //maybe have a get_vol method here, since we will find the fair vol here

    
    private:
    protected:
}
*/

#endif
