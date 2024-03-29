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

struct extract {
    std::vector<std::string> upper_bounds_;
    std::vector<std::string> lower_bounds_;
    std::vector<int> values_;
    unsigned int size() const {return values_.size();}
};

struct visualization_data {
  std::vector<std::string> str_values_;
  std::vector<int> scale_factors_;

  size_t lower_bounds_width_ = 0; 
  size_t upper_bounds_width_ = 0;
  size_t str_values_width_ = 0;
  size_t external_line_shift_ = 0; 
  visualization_data(const std::vector<std::string>& str_values, 
                     const std::vector<int>& scale_factors,
                     const size_t& lower_bounds_width,
                     const size_t& upper_bounds_width, 
                     const size_t& str_values_width,
                     const size_t& external_line_shift)
                     :
                     str_values_{str_values}, 
                     scale_factors_{scale_factors},
                     lower_bounds_width_{lower_bounds_width},
                     upper_bounds_width_{upper_bounds_width},
                     str_values_width_{str_values_width},
                     external_line_shift_{external_line_shift}
                     {}
};

template <class histogram>
extract extract_data(const histogram& h) {
  std::string lower, upper;

  auto data = indexed(h, coverage::all);

  extract ex;  
  for (auto x : data) {
    lower = str( boost::format("%.1f") % x.bin().lower() );
    upper = str( boost::format("%.1f") % x.bin().upper() );
    ex.lower_bounds_.push_back(lower);
    ex.upper_bounds_.push_back(upper);
    ex.values_.push_back(*x);
  }
  return ex;    
}

std::string get_single_label(const extract& data,
                             const unsigned int index, 
                             const unsigned int column_width1,
                             const unsigned int column_width2) {
  std::string label = "";
  char parenthesis = ' ';
  std::string lower = data.lower_bounds_.at(index);
  std::string upper = data.upper_bounds_.at(index);
  if (index == data.size() - 1)
    parenthesis = ']';
  else
    parenthesis = ')';

  label = '[' 
        + str(boost::format("%s") % boost::io::group(std::setw(column_width1), lower))
        + ", "
        + str(boost::format("%s") % boost::io::group(std::setw(column_width2), upper))
        + parenthesis;
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
    if (line.length() > max_length) 
      max_length = line.length();
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

visualization_data precalculate_visual_data(extract& h_data){
  const unsigned int additional_offset = 6; // 6 white characters  
  const auto scale_factors = calculate_scale_factors(h_data.values_);
  const auto str_values = convert_to_str_vec(h_data.values_);
  const auto lower_width = get_max_width(h_data.lower_bounds_); 
  const auto upper_width = get_max_width(h_data.upper_bounds_);
  const auto str_values_width = get_max_width(str_values);
  const auto hist_shift = lower_width + upper_width + str_values_width + additional_offset;

  visualization_data v_data(str_values, scale_factors, lower_width, upper_width,
                            str_values_width, hist_shift);
  return v_data;
}

std::string draw_histogram(const extract& h_data, const visualization_data& v_data) {
  std::stringstream visualisation;
  
  visualisation << "\n" << get_external_line(v_data.external_line_shift_) << "\n";

  for (unsigned int i = 0; i < h_data.size(); i++)
    visualisation << get_single_label(h_data, i, v_data.lower_bounds_width_, v_data.upper_bounds_width_) << "  "
                  << get_single_str_value(v_data.str_values_, i, v_data.str_values_width_) << " "
                  << get_single_histogram_line(v_data.scale_factors_, i) << "\n";
                   
  visualisation << get_external_line(v_data.external_line_shift_) << "\n\n";
  return visualisation.str();
}
} // namespace

template <class histogram>
void display(const histogram& h) {

  auto histogram_data = extract_data(h);
  auto visualization_data = precalculate_visual_data(histogram_data);

  std::cout << draw_histogram(histogram_data, visualization_data);
}

} // namespace display

#endif