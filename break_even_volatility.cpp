#include "break_even_volatility.hpp"

/*******************************************
   *time series constructor & destructor*
********************************************/

time_series::time_series(const std::string& filepath, const std::string& dataname)
    : m_filepath(filepath), m_dataname(dataname)
{
    // read data from the csv file (with column 1 as dates and column 2 as data)
    std::ifstream file(m_filepath);
    std::string line;
    std::string cell;
    while(std::getline(file,line)) // do if there is a line to read
    {
        std::stringstream lineStream(line); // convert the line from a string to a stringstream
        
        // date part
        std::getline(lineStream,cell,','); // read the first part (separating the stringstream by ',')
        m_datadate.push_back(c_str_timet(cell)); // store all the dates (in time_t) in the private member m_datadate
        
        // data part
        std::getline(lineStream,cell,','); // read the second part of the stringstream
        m_data.push_back(std::stod(cell)); // store all data (in double) in the private member m_data
    }
}

time_series::~time_series()
{
}

/*******************************************
           *time series methods*
********************************************/

// method to get private data member m_dataname
const std::string& time_series::get_dataname() const
{
    return m_dataname;
}

// methods to get the whole series of data
const std::vector<double>& time_series::get_data() const
{
    return m_data;
}

const std::vector<time_t>& time_series::get_date() const
{
    return m_datadate;
}

// method to determine the starting and ending positions of the target data series
std::vector<ptrdiff_t> time_series::get_datapos(const std::string& maturity, const size_t& term_day) const
{
    std::vector<ptrdiff_t> result(2);
    time_t maturityt = c_str_timet(maturity); // convert the maturity time string to time_t object
    ptrdiff_t endpos = std::find(m_datadate.begin(), m_datadate.end(), maturityt) - m_datadate.begin(); // find the position of maturity date
    if (endpos >= m_datadate.size()) // check if the end date is in the data series
    {
        std::cout << "Target date is not found in the data!!" << std::endl;
        return result; // !!TO DO: Replace by error!
    }
    else
    {
        ptrdiff_t startpos;
        int daycount = 0;
        bool startfound = false;
        while(daycount <= 7)
        {
            time_t startt = maturityt - (term_day + daycount)*24*60*60; // define the starting date by end date - number of calendar days (- daycount if previous date doesn't exist in the data series)
            startpos = std::find(m_datadate.begin(), m_datadate.end(), startt) - m_datadate.begin();
            if (startpos < m_datadate.size())
            {
                startfound = true;
                break;
            }
            daycount += 1;
        }
        
        if(startfound == false)
        {
            std::cout << "Not enough historical data!!" << std::endl;
            return result; // !!TO DO: Replace by error!
        }
        else
        {
            result = {startpos, endpos};
            return result;
        }
    }
}

// method to get a part of the series (data) with starting and ending positions
std::vector<double> time_series::get_data(const ptrdiff_t& startpos, const ptrdiff_t& endpos) const
{
    std::vector<double> result(m_data.begin() + startpos, m_data.begin() + endpos + 1);
    return result;
}

// method to get a part of the series (date) with starting and ending positions
std::vector<time_t> time_series::get_date(const ptrdiff_t& startpos, const ptrdiff_t& endpos) const
{
    std::vector<time_t> result(m_datadate.begin() + startpos, m_datadate.begin() + endpos + 1);
    return result;
}


/*******************************************
     *option constructor & destructor*
********************************************/

option::option(const time_series& underlying, const double& strike, const double& vol, const time_series& rate, const std::string& maturity, const size_t& term_day, const int& type)
    : m_underlying(underlying), m_strike(strike), m_vol(vol), m_maturity(maturity), m_term_day(term_day), m_type(type)
{
    std::cout<<"Option Constructor"<<std::endl;

    // store the position of data (target period) in the whole underlying series
    m_datapos = m_underlying.get_datapos(m_maturity, m_term_day);
    
    // create a vector of interest rate data with dates match with those of the underlying data
    std::vector<time_t> data_date = m_underlying.get_date(m_datapos[0], m_datapos[1]); // get the dates of the target underlying data
    
    std::vector<time_t> rate_date = rate.get_date(); // get the dates for the whole interest rate series
    std::vector<double> rate_data = rate.get_data(); // get the whole interest rate series
    
    ptrdiff_t target_pos;
    time_t target_date;
    
    for(int i = 0; i < data_date.size(); i++) // loop through all the dates of target underlying data to find the corresponding rates
    {
        int day_count = 0;
        bool target_found = false;
        while(day_count <= 7) // limit the dateback for interest rate search with 7 days
        {
            target_date = data_date[i] - day_count*24*60*60; // set the target date as the date in underlying series
            target_pos = std::find(rate_date.begin(), rate_date.end(), target_date) - rate_date.begin(); // find the position of the corresponding date in the rate series
            if (target_pos < rate_date.size())
            {
                target_found = true;
                break; // quit the while loop if the date is found
            }
            day_count += 1; // if no interest rate is found for a given day, try to look at the day before
        }
        if(target_found == false)
        {
            std::cout << "Missing interest rate data at " << i << std::endl; // if the date is not found
            m_fixedrate.push_back(0); // !!TO DO: Replace by error!
        }
        else
        {
            m_fixedrate.push_back(rate_data[target_pos]); // store the interest rate as of the corresponding date
        }
    }
}

option::option(const time_series& underlying, const double& strike, const double& vol, const double& rate, const std::string& maturity, const size_t& term_day, const int& type)
    : m_underlying(underlying), m_strike(strike), m_vol(vol), m_maturity(maturity), m_term_day(term_day), m_type(type)
{
    std::cout<<"Option Constructor"<<std::endl;
    
    // store the position of data (target period) in the whole underlying series
    m_datapos = m_underlying.get_datapos(m_maturity, m_term_day);
    
    // create a vector of interest rate data with dates match with those of the underlying data
    std::vector<time_t> data_date = m_underlying.get_date(m_datapos[0], m_datapos[1]); // get the dates of the target underlying data
    
    m_fixedrate.assign(data_date.size(), rate); // fill the interest rate vector with the constant rate
}

option::~option()
{
}

/*******************************************
              *option methods*
********************************************/

std::vector<double> option::BS_price() const
{
    std::vector<double> underlying_data = m_underlying.get_data(m_datapos[0], m_datapos[1]);

    int sign;
    
    if(m_type == 1)
    {
	    sign = 1;
    }
    else if(m_type == 0)
    {
	    sign = -1;
    }
	
    // create a vector to store time to maturity in year (based on trading days)
    std::vector<double> time_to_maturity(underlying_data.size());
    std::iota(time_to_maturity.begin(), time_to_maturity.end(), 1); // fill the vector with increasing number from 1,2,3,...
    std::transform(time_to_maturity.begin(), time_to_maturity.end(), time_to_maturity.begin(), [underlying_data](double& arg){return (underlying_data.size() - arg) / 252.;}); // revert the vector from ascending order to descending order and change the unit to year
    
    std::vector<double> option_price;
    double d1;
    double price;
    double d2;
    
    for(size_t i = 0; i < underlying_data.size(); i++) // calculate the option price on each trading day
    {
        if(i != underlying_data.size() - 1) // option price before maturity
        {
            d1 = 1. / (m_vol * sqrt(time_to_maturity[i])) * (log(underlying_data[i] / m_strike) + (m_fixedrate[i] + pow(m_vol, 2.) / 2.) * time_to_maturity[i]);
            d2 = d1 - m_vol * sqrt(time_to_maturity[i]);
            price = sign * (normalCDF(sign * d1) * underlying_data[i] - sign * normalCDF(sign * d2) * m_strike * exp(-m_fixedrate[i] * time_to_maturity[i]));
            option_price.push_back(price);
        }
        else // call option payoff at maturity
        {
            price = std::max((sign * underlying_data[i] - m_strike), 0.);
            option_price.push_back(price);
        }
    }
    
    return option_price;
}

std::vector<double> option::BS_delta() const
{
    std::vector<double> underlying_data = m_underlying.get_data(m_datapos[0], m_datapos[1]);
    
    // create a vector to store time to maturity in year (based on trading days)
    std::vector<double> time_to_maturity(underlying_data.size());
    std::iota(time_to_maturity.begin(), time_to_maturity.end(), 1); // fill the vector with increasing number from 1,2,3,...
    std::transform(time_to_maturity.begin(), time_to_maturity.end(), time_to_maturity.begin(), [underlying_data](double& arg){return (underlying_data.size() - arg) / 252.;}); // revert the vector from ascending order to descending order and change the unit to year
    
    std::vector<double> option_delta;
    double d1;
    double delta;
    
    for(int i = 0; i < underlying_data.size(); i++) // calculate the option delta on each trading day
    {

        if(i != underlying_data.size() - 1) // option delta before maturity
        {
            d1 = 1. / (m_vol * sqrt(time_to_maturity[i])) * (log(underlying_data[i] / m_strike) + (m_fixedrate[i] + pow(m_vol, 2.) / 2.) * time_to_maturity[i]);
            delta = normalCDF(d1);
	    
	    if(m_type == 0)
	    {
		    delta = delta - 1;
	    }

            option_delta.push_back(delta);
	    
        }
        else // delta at maturity
        {
            option_delta.push_back(0);
        }
    }
    
    return option_delta;
}

void option::modify_vol(double vol)
{
	m_vol = vol;
}

const double option::get_volatility()
{
	return m_vol;
}

void option::modify_strike(double strike)
{
    m_strike = strike;
}


/*******************************************
          *auxillary functions*
********************************************/

// function converting date string to time_t object
time_t c_str_timet(const std::string& targetdate)
{
    struct tm temp = {0};
    
    std::istringstream tt(targetdate); // convert the date string to stringstream
    tt >> std::get_time(&temp, "%d/%m/%Y"); // convert the date stringstream to struct tm
    
    time_t result = std::mktime(&temp);
    return result; // convert struct tm to time_t
}

// function return Normal CDF N(x)
double normalCDF(const double& x)
{
    return std::erfc(-x/sqrt(2))/2;
}
