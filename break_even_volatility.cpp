#include "break_even_volatility.hpp"

//time series constructor & destructor
time_series::time_series(const std::string& filepath, const std::string& dataname)
:m_filepath(filepath), m_dataname(dataname)
{
    std::ifstream file(m_filepath);
    std::string line;
    std::string cell;
    while(std::getline(file,line))
    {
        std::stringstream lineStream(line);
        std::getline(lineStream,cell,',');
        m_datadate.push_back(cell);
        std::getline(lineStream,cell,',');
        m_data.push_back(std::stod(cell));
    }
}

time_series::~time_series()
{
}

//time series methods
std::vector<double> time_series::get_data() const
{
    return m_data;
}

std::vector<double> time_series::get_data(const std::string& maturity, const size_t& day_number) const
{
    ptrdiff_t pos = std::find(m_datadate.begin(), m_datadate.end(), maturity) - m_datadate.begin();
    if (pos>= m_datadate.size())
    {
        std::cout << "Target date is not found in the data!!" << std::endl;
        return 0; // Replace by error!
    }
    else
    {
        if (day_number >= pos + 1)
        {
            std::cout << "Not enough historical data!!" << std::endl;
            return 0; // Replace by error!
        }
        else
        {
            return std::vector<double> result(m_data.begin() + pos - day_number, m_data.begin() + pos)
        }
    }
}

std::vector<size_t> day_to_maturity(const std::string& maturity, const size_t& day_number) const
{
    std::vector<size_t> result(day_number + 1);
    std::iota(result.begin(), result.end(), 0);
    std::for_each(result.begin(), result.end(), [](size_t& x) {x = day_number - x;});
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
