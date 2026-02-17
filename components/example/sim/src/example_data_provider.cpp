#include <example_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(ExampleDataProvider,"EXAMPLE_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    ExampleDataProvider::ExampleDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("ExampleDataProvider::ExampleDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> ExampleDataProvider::get_data_point(void) const
    {
        sim_logger->trace("ExampleDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new ExampleDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
