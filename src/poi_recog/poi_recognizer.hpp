#ifndef __POI_RECOGNIZER__
#define __POI_RECOGNIZER__

#include "dg_core.hpp"
#include "utils/python_embedding.hpp"
#include <fstream>
#include <chrono>


using namespace std;

namespace dg
{
    struct POIResult
    {
        int xmin, ymin, xmax, ymax;
        std::string label;
        double confidence;
    };

    /**
    * @brief C++ Wrapper of Python module - POIRecognizer
    */
    class POIRecognizer : public PythonModuleWrapper
    {
    public:
        /**
        * Initialize the module
        * @return true if successful (false if failed)
        */
        bool initialize(const char* module_name = "poi_recognizer", const char* module_path = "./../src/poi_recog", const char* class_name = "POIRecognizer", const char* func_name_init = "initialize", const char* func_name_apply = "apply")
        {
            dg::Timestamp t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;

            PyGILState_STATE state;
            if (isThreadingEnabled()) state = PyGILState_Ensure();

            bool ret = _initialize(module_name, module_path, class_name, func_name_init, func_name_apply);

            if (isThreadingEnabled()) PyGILState_Release(state);

            dg::Timestamp t2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
            m_processing_time = t2 - t1;

            return ret;
        }

        /**
        * Reset variables and clear the memory
        */
        void clear()
        {
            PyGILState_STATE state;

            if (isThreadingEnabled()) state = PyGILState_Ensure();

            _clear();

            if (isThreadingEnabled()) PyGILState_Release(state);

            m_pois.clear();
            m_timestamp = -1;
            m_processing_time = -1;
        }

        /**
        * Run once the module for a given input (support thread run)
        * @return true if successful (false if failed)
        */
        bool apply(cv::Mat image, dg::Timestamp t)
        {
            dg::Timestamp t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;

            PyGILState_STATE state;
            if (isThreadingEnabled()) state = PyGILState_Ensure();

            bool ret = _apply(image, t);

            if (isThreadingEnabled()) PyGILState_Release(state);

            dg::Timestamp t2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
            m_processing_time = t2 - t1;

            return ret;
        }

        /**
        * Run once the module for a given input
        * @return true if successful (false if failed)
        */
        bool _apply(cv::Mat image, dg::Timestamp ts)
        {
            // Set function arguments
            int arg_idx = 0;
            PyObject* pArgs = PyTuple_New(2);

            // Image
            import_array();
            npy_intp dimensions[3] = { image.rows, image.cols, image.channels() };
            PyObject* pValue = PyArray_SimpleNewFromData(image.dims + 1, (npy_intp*)&dimensions, NPY_UINT8, image.data);
            if (!pValue) {
                fprintf(stderr, "POIRecognizer::apply() - Cannot convert argument1\n");
                return false;
            }
            PyTuple_SetItem(pArgs, arg_idx++, pValue);

            // Timestamp
            pValue = PyFloat_FromDouble(ts);
            PyTuple_SetItem(pArgs, arg_idx++, pValue);

            // Call the method
            PyObject* pRet = PyObject_CallObject(m_pFuncApply, pArgs);
            if (pRet != NULL) {
                Py_ssize_t n_ret = PyTuple_Size(pRet);
                if (n_ret != 2)
                {
                    fprintf(stderr, "POIRecognizer::apply() - Wrong number of returns\n");
                    return false;
                }

                // list of list
                m_pois.clear();
                PyObject* pList0 = PyTuple_GetItem(pRet, 0);
                if (pList0 != NULL)
                {
                    Py_ssize_t cnt = PyList_Size(pList0);
                    for (int i = 0; i < cnt; i++)
                    {
                        PyObject* pList = PyList_GetItem(pList0, i);
                        if(pList)
                        {
                            POIResult poi;
                            int idx = 0;
                            pValue = PyList_GetItem(pList, idx++);
                            poi.xmin = PyLong_AsLong(pValue);
                            pValue = PyList_GetItem(pList, idx++);
                            poi.ymin = PyLong_AsLong(pValue);
                            pValue = PyList_GetItem(pList, idx++);
                            poi.xmax = PyLong_AsLong(pValue);
                            pValue = PyList_GetItem(pList, idx++);
                            poi.ymax = PyLong_AsLong(pValue);
                            pValue = PyList_GetItem(pList, idx++);
                            poi.label = PyUnicode_AsUTF8(pValue);
                            pValue = PyList_GetItem(pList, idx++);
                            poi.confidence = PyFloat_AsDouble(pValue);
                            m_pois.push_back(poi);
                        }
                    }
                }
            }
            else {
                PyErr_Print();
                fprintf(stderr, "POIRecognizer::apply() - Call failed\n");
                return false;
            }

            // Update Timestamp
            m_timestamp = ts;

            // Clean up
            if(pRet) Py_DECREF(pRet);
            if(pArgs) Py_DECREF(pArgs);            

            return true;
        }

        void get(std::vector<POIResult>& pois)
        {
            pois = m_pois;
        }

        void get(std::vector<POIResult>& pois, Timestamp& ts)
        {
            pois = m_pois;
            ts = m_timestamp;
        }

        void set(const std::vector<POIResult>& pois, Timestamp ts, double proc_time)
        {
            m_pois = pois;
            m_timestamp = ts;
            m_processing_time = proc_time;
        }

        dg::Timestamp timestamp() const
        {
            return m_timestamp;
        }

        double procTime() const
        {
            return m_processing_time;
        }

        void draw(cv::Mat& image, cv::Scalar color = cv::Scalar(0, 255, 0), int width = 2) const
        {
            for (size_t i = 0; i < m_pois.size(); i++)
            {
                cv::Rect rc(m_pois[i].xmin, m_pois[i].ymin, m_pois[i].xmax - m_pois[i].xmin + 1, m_pois[i].ymax - m_pois[i].ymin + 1);
                cv::rectangle(image, rc, color, width);
                cv::Point pt(m_pois[i].xmin + 5, m_pois[i].ymin + 35);
                std::string msg = cv::format("%s %.2lf", m_pois[i].label.c_str(), m_pois[i].confidence);
                cv::putText(image, msg, pt, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0), 6);
                cv::putText(image, msg, pt, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 0), 2);
            }
        }

        void print() const
        {
            printf("[%s] proctime = %.3lf, timestamp = %.3lf\n", name(), procTime(), m_timestamp);
            for (int k = 0; k < m_pois.size(); k++)
            {
                printf("\t%s, %.2lf, x1=%d, y1=%d, x2=%d, y2=%d\n", m_pois[k].label.c_str(), m_pois[k].confidence, m_pois[k].xmin, m_pois[k].ymin, m_pois[k].xmax, m_pois[k].ymax);
            }
        }

        void write(std::ofstream& stream, int cam_fnumber = -1) const
        {
            for (int k = 0; k < m_pois.size(); k++)
            {
                std::string log = cv::format("%.3lf,%d,%s,%s,%.2lf,%d,%d,%d,%d,%.3lf", m_timestamp, cam_fnumber, name(), m_pois[k].label.c_str(), m_pois[k].confidence, m_pois[k].xmin, m_pois[k].ymin, m_pois[k].xmax, m_pois[k].ymax, m_processing_time);
                stream << log << std::endl;
            }
        }

        static const char* name()
        {
            return "poi";
        }


    protected:
        std::vector<POIResult> m_pois;
        Timestamp m_timestamp = -1;
        double m_processing_time = -1;
    };

} // End of 'dg'

#endif // End of '__POI_RECOGNIZER__'
