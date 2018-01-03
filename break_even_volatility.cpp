#include "break_even_volatility.hpp"

//time series constructor & destructor
time_series::time_series(const std::string& filepath, const std::string& dataname)
:m_filepath(filepath), m_dataname(dataname)
{
    // read data from the csv file (with column 1 as dates and column 2 as data)
    // !!TO DO: change the date datatype from string to datetime
    std::ifstream file(m_filepath);
    std::string line;
    std::string cell;
    while(std::getline(file,line)) // do if there is a line to read
    {
        std::stringstream lineStream(line); // convert the line from a string to a stringstream
        std::getline(lineStream,cell,','); // read the first part (separating the stringstream by ',')
        m_datadate.push_back(cell); // store each date in the private member m_datadate
        std::getline(lineStream,cell,','); // read the second part of the stringstream
        m_data.push_back(std::stod(cell)); // store each data in the private member m_data
    }
}

time_series::~time_series()
{
}

//time series methods
std::vector<std::string> time_series::get_dataname() const // get private data member m_dataname
{
    return m_dataname;
}

std::vector<double> time_series::get_data() const // get the whole series of data
{
    return m_data;
}

std::vector<double> time_series::get_data(const std::string& maturity, const size_t& day_number) const // get a part of the series with the end date and target number of days (for instance, 5 data points are extracted for day_number = 4)
{
    ptrdiff_t pos = std::find(m_datadate.begin(), m_datadate.end(), maturity) - m_datadate.begin(); // find the position of end date
    if (pos>= m_datadate.size()) // check if the end date is in the data series
    {
        std::cout << "Target date is not found in the data!!" << std::endl;
        return 0; // !!TO DO: Replace by error!
    }
    else
    {
        if (day_number >= pos + 1) // check if the target number of days exceeds the available number of data points
        {
            std::cout << "Not enough historical data!!" << std::endl;
            return 0; // !!TO DO: Replace by error!
        }
        else
        {
            return std::vector<double> result(m_data.begin() + pos - day_number, m_data.begin() + pos) // return the required data
        }
    }
}

std::vector<size_t> day_to_maturity(const std::string& maturity, const size_t& day_number) const // create a vector containing daily days to maturity
{
    std::vector<size_t> result(day_number + 1); // create a vector with size day_number+1
    std::iota(result.begin(), result.end(), 0); // fill the vector with increasing number from 0,1,2,3,... to day_number
    std::for_each(result.begin(), result.end(), [](size_t& x) {x = day_number - x;}); // revert the vector from ascending order to descending order
    return result;
}
/*
//pricer constructor & destructor
pricer::pricer(const time_series& underlying, const double& strike, const double& vol, const time_series& rate, const size_t& maturity)
: m_underlying(underlying), m_strike(strike), m_vol(vol), m_rate(rate), m_maturity(maturity)
{
}

pricer::~pricer()
{
}

std::vector<double> pricer::BS_price() const
{
    std::vector<double> timeToMaturity = 
    double d1 = 1/(m_vol)
}
*/
