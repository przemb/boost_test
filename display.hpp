#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <boost/format.hpp>
#include <boost/histogram.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace display {
namespace {
using namespace boost::histogram;

const unsigned int histogram_width = 60; // 60 characters
const float max_bin_coefficient = 0.95;  // 95% of histogram_width

template <class histogram>
void extract_data(const histogram& h, std::vector<std::string>& labels,
                  std::vector<int>& values) {
  std::string label = "";
  auto data = indexed(h, coverage::all);

  for (auto x : data) {
    label = str(boost::format("[%-.1f, %.1f") % x.bin().lower() % x.bin().upper());
    labels.push_back(label);
    values.push_back(*x);
  }
}

std::string get_single_label(const std::vector<std::string>& labels,
                             const unsigned int index, const unsigned int column_width) {
  std::string label = "";

  if (index == labels.size() - 1)
    label = labels.at(index) + ']';
  else
    label = labels.at(index) + ')';

  label = str(boost::format("%-s") % boost::io::group(std::setw(column_width), label));
  return label;
}

std::string get_single_str_value(const std::vector<std::string>& str_values,
                                 const unsigned int index,
                                 const unsigned int column_width) {
  std::string str_value = str_values.at(index);
  str_value =
      str(boost::format("%-s") % boost::io::group(std::setw(column_width), str_value));
  return str_value;
}

std::vector<int> calculate_scale_factors(const std::vector<int>& values) {
  std::vector<int> scale_factors{};
  const unsigned int longest_bin = max_bin_coefficient * histogram_width;

  auto max_value = std::max_element(values.begin(), values.end());

  for (const auto& x : values) {
    int result = x * longest_bin / (*max_value);
    scale_factors.push_back(result);
  }
  return scale_factors;
}

size_t get_max_width(const std::vector<std::string>& container) {
  size_t max_length = 0;

  for (const auto& line : container)
    if (line.length() > max_length) max_length = line.length();
  return max_length;
}

std::vector<std::string> convert_to_str_vec(const std::vector<int>& values) {
  std::vector<std::string> string_values;
  string_values.reserve(values.size());

  std::transform(values.begin(), values.end(), std::back_inserter(string_values),
                 [](int i) { return std::to_string(i); });
  return string_values;
}

std::string draw_line(const unsigned int num, const char c = '*', bool complete = true) {
  std::stringstream line;
  unsigned int i = 0;
  for (; i < num; ++i) line << c;

  if (complete == true) {
    for (; i < histogram_width; ++i) line << ' ';
  }
  return line.str();
}

std::string get_single_histogram_line(const std::vector<int>& values,
                                      const unsigned int index) {
  std::stringstream line;
  line << "|" << draw_line(values.at(index)) << '|';
  return line.str();
}

std::string get_external_line(const unsigned int labels_width) {
  std::stringstream external_line;

  external_line << draw_line(labels_width, ' ', false) << " +"
                << draw_line(histogram_width, '-') << '+';
  return external_line.str();
}

std::string get_top_line(const unsigned int labels_width,
                         const std::vector<int>& values) {
  std::stringstream top_line;

  const unsigned int min = 0;
  auto max_value = std::max_element(values.begin(), values.end());
  const float max = *max_value / max_bin_coefficient;

  std::string min_max = str(boost::format("%-i %.1f") % min %
                            boost::io::group(std::setw(histogram_width), max));

  top_line << draw_line(labels_width, ' ', false) << " " << min_max;
  return top_line.str();
}

std::string draw_histogram(const std::vector<std::string>& labels,
                           const std::vector<int>& values) {
  const auto scale_factors = calculate_scale_factors(values);
  const auto str_values = convert_to_str_vec(values);
  const auto labels_width = get_max_width(labels) + 1; // + 1 for parenthesis
  const auto str_values_width = get_max_width(str_values);
  const auto hist_shift = labels_width + str_values_width + 1; // + 1 for " "

  std::stringstream visualisation;
  visualisation << "\n" << get_external_line(hist_shift) << "\n";

  for (unsigned int i = 0; i < values.size(); i++)
    visualisation << get_single_label(labels, i, labels_width) << " "
                  << get_single_str_value(str_values, i, str_values_width) << " "
                  << get_single_histogram_line(scale_factors, i) << "\n";

  visualisation << get_external_line(hist_shift) << "\n\n";
  return visualisation.str();
}
} // namespace

template <class histogram>
void display(const histogram& h) {
  std::vector<std::string> labels{};
  std::vector<int> values{};

  extract_data(h, labels, values);

  std::cout << draw_histogram(labels, values);
}

} // namespace display

#endif