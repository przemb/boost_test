#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <vector>
#include <string>
#include <boost/histogram.hpp>

#include "display.hpp"
#include <boost/histogram.hpp>
#include <boost/format.hpp> // used here for printing
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace display
{
using namespace boost::histogram;

const unsigned int histogram_width = 60; // 60 characters 
const float max_bin_coefficient = 0.95; // 95%

template <class histogram>
void preapre_labels(const histogram& h, std::vector<std::string>& labels, std::vector<int>& values)
{
    const auto lower_bound = h.axis().begin()->lower();
    const auto upper_bound = h.axis().rbegin()->upper();

    std::string s1 = str(boost::format("[-inf, %i) %|13t|%2i") % lower_bound % 0);
    labels.push_back(s1);
    values.push_back(0);

    for (auto x : indexed(h))
    {
        std::string s2 = str(boost::format("[%-3.1f, %.1f) %|13t|%2i") % x.bin().lower() % x.bin().upper() % *x);
        labels.push_back(s2);
        values.push_back(*x);
    }
    std::string s3 = str(boost::format("[%i, inf] %|13t|%2i") % upper_bound % 0);
    labels.push_back(s3);
    values.push_back(0);
}

void print_all_labels(const std::vector<std::string>& labels)
{
    for (const auto &x : labels)
        std::cout << x << "\n";
}

std::string get_single_label(const std::vector<std::string>& labels, int index)
{
    return labels.at(index);
}


std::vector<int> calculate_scale_factors(const std::vector<int>& values)
{
    std::vector<int> scale_factors {};
    const unsigned int longest_bin = max_bin_coefficient * histogram_width;
    
    auto max_value = std::max_element(values.begin(), values.end());
      
    for(const auto& x : values)
    {
        int result = x * longest_bin / (*max_value);
        scale_factors.push_back(result);
    }

    return scale_factors;
}

std::string draw_line(const unsigned int num, const char c = '*', bool complete = true)
{
    std::stringstream line;
    int i = 0;
    for(; i < num; ++i)
        line << c;
    
    if(complete == true)
    {
        for(; i < histogram_width; ++i)
            line << ' ';
    }

    return line.str();
}


std::string get_single_line(const std::vector<int>& values, const unsigned int index)
{
    return draw_line(values.at(index));
}

std::string get_single_histogram_line(const std::vector<int>& values, const unsigned int index)
{
    std::stringstream line;
    line << "|" << get_single_line(values, index) << '|';
    return line.str();
}

std::string get_external_line(std::vector<std::string>& labels)
{
    std::stringstream external_line;
    auto label_width = labels[0].size();
    external_line << draw_line(label_width, ' ', false) << " +" << draw_line(histogram_width, '-') << '+';

    return external_line.str();
}

std::string get_top_line(std::vector<std::string>& labels, std::vector<int>& values)
{
    std::stringstream top_line;
    auto label_width = labels[0].size();
    const unsigned int min = 0;
    auto max_value = std::max_element(values.begin(), values.end());
    const float max = *max_value / max_bin_coefficient;
    std::string min_max = str(boost::format("%-i %.1f") % min % boost::io::group(std::setw(histogram_width), max));

    top_line << draw_line(label_width, ' ', false) << " " <<  min_max;

    return top_line.str();
}

void draw_histogram(std::vector<std::string>& labels, std::vector<int>& values)
{   
    auto scale_factors = calculate_scale_factors(values);

    std::cout << "\n" << get_top_line(labels, values) << "\n";
    std::cout << get_external_line(labels) << "\n";
    
    for(int i = 0; i < values.size(); i++)
        std::cout << get_single_label(labels, i) << " " << get_single_histogram_line(scale_factors, i) <<  "\n";
    
    std::cout << get_external_line(labels) << "\n\n";
}       

template <class histogram>
void display(const histogram& h)
{
    std::vector<std::string> labels {}; 
    std::vector<int> values {};
    
    preapre_labels(h, labels, values);
    //print_all_labels(labels);
    //std::cout << get_single_label(labels, 1);
    
    //std::cout << get_single_histogram_line(values, 1);
    draw_histogram(labels, values);
}




} // namespace display

#endif