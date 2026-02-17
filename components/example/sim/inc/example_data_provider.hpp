#ifndef NOS3_EXAMPLEDATAPROVIDER_HPP
#define NOS3_EXAMPLEDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <example_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class ExampleDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        ExampleDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~ExampleDataProvider(void) {};
        ExampleDataProvider& operator=(const ExampleDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
