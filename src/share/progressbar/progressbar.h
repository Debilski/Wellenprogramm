/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 encoding=utf-8: */
#include <iostream>
#include <cstring>
#include <fstream>
// using namespace std;

class ProgressBar
{

    long long max;
    long long percentage;
    std::string ndots;
    std::string infostring;

public:
    ProgressBar(long long max);

    ProgressBar(long long max, const std::string& infostring);

    void print(long long status, const std::string& additional = std::string());
    bool prints_now(long long status);

private:
    void p_print(long long status, const std::string& additional = std::string());
};

inline bool ProgressBar::prints_now(long long status)
{
    return (percentage != 100 && percentage != (status + 1) * 100 / max);
}

inline void ProgressBar::print(long long status, const std::string& additional)
{
    // if (status*100/max != status) {
    if (this->prints_now(status)) {
        p_print(status, additional);
    }
}
