/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 encoding=utf-8: */
#include "progressbar.h"

ProgressBar::ProgressBar(long long max):
    max(max),
    percentage(0),
    ndots("                         "),
    infostring("Processing")
{
}

ProgressBar::ProgressBar(long long max, const std::string& infostring):
    max(max),
    percentage(0),
    ndots("                         "),
    infostring(infostring)
{
}

void ProgressBar::p_print(long long status, const std::string& additional)
{
  percentage = (status + 1) * 100 / max;
  if (percentage % 2 == 1) {
    ndots[percentage / 4] = '.';
  } else {
    ndots[percentage / 4] = '#';
  }
  std::cout << infostring << ": [" << ndots << "] " <<
  percentage << "%" << additional << "\r" << std::flush;
  if (percentage == 100)
    std::cout << "\n" << std::flush;
}
