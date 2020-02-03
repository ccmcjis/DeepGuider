#include "dg_core.hpp"
#include "python_embedding.hpp"
#include "python_test.hpp"
#include <chrono>

using namespace dg;
using namespace std;

int main()
{
	// Initialize the Python interpreter
    init_python_environment();

	// Initialize Python module
	PythonTest python_test;
	if (!python_test.initialize())
	{
		return -1;
	}

	// Run the Python module
	cv::Mat image = cv::imread("poi_sample.jpg");
	int nIter = 5;
	for (int i = 1; i <= nIter; i++)
	{
        Timestamp t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;

		if (!python_test.apply(image, t)) {
			return -1;
		}

        std::vector<TestResult> result;
        python_test.get(result);
        Timestamp t2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
        printf("iteration: %d (it took %lf seconds)\n", i, t2 - t);
        for (int k = 0; k < result.size(); k++)
        {
            printf("\t[%d] sim_theshold=%.1lf, output_txt=%s, t=%lf\n", k, result[k].sim_threshold, result[k].sim_threshold, t);
        }
	}

	// Clear the Python module
	python_test.clear();

	// Close the Python Interpreter
    close_python_environment();

	return 0;
}