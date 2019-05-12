#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <boost/histogram.hpp>
#include <boost/format.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace display {
using namespace boost::histogram;

const unsigned int histogram_width = 60; // 60 characters 
const float max_bin_coefficient = 0.95; // 95% of histogram_width

template <class histogram>
void extract_data(const histogram& h, std::vector<std::string>& labels, std::vector<int>& values) {

    std::string tmp = "";
    auto data = indexed(h, coverage::all);

    for (auto x : data) {
        tmp = str(boost::format("[%-g, %g") % x.bin().lower() % x.bin().upper() );
        labels.push_back(tmp);
        values.push_back(*x);
    }
}

void print_all_labels(const std::vector<std::string>& labels) {
    for (const auto &x : labels)
        std::cout << x << "\n";
}

std::string get_single_label(const std::vector<std::string>& labels, const unsigned int index, const unsigned int labels_width){
    std::string label;

    if(index == labels.size() - 1)
        label = labels.at(index) + ']';
    else 
        label = labels.at(index) + ')';

    label = str(boost::format("%-s") % boost::io::group(std::setw(labels_width), label));
    
    return label;
}


std::vector<int> calculate_scale_factors(const std::vector<int>& values) {
    std::vector<int> scale_factors {};
    const unsigned int longest_bin = max_bin_coefficient * histogram_width;
    
    auto max_value = std::max_element(values.begin(), values.end());
      
    for(const auto& x : values){
        int result = x * longest_bin / (*max_value);
        scale_factors.push_back(result);
    }

    return scale_factors;
}

size_t get_max_width(const std::vector<std::string>& container) {
    size_t max_length = 0;
    
    for(const auto& line : container)
        if(line.length() > max_length)
            max_length = line.length();
    
    return max_length + 1; // + 1 for parenthesis
}
    

std::string draw_line(const unsigned int num, const char c = '*', bool complete = true) {
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


std::string get_single_line(const std::vector<int>& values, const unsigned int index) {
    return draw_line(values.at(index));
}

std::string get_single_histogram_line(const std::vector<int>& values, const unsigned int index) {
    std::stringstream line;
    line << "|" << get_single_line(values, index) << '|';
    return line.str();
}

std::string get_external_line(const unsigned int labels_width) {
    std::stringstream external_line;

    external_line << draw_line(labels_width, ' ', false) << " +" << draw_line(histogram_width, '-') << '+';

    return external_line.str();
}

std::string get_top_line(const unsigned int labels_width, const std::vector<int>& values) {
    std::stringstream top_line;

    const unsigned int min = 0;
    auto max_value = std::max_element(values.begin(), values.end());
    const float max = *max_value / max_bin_coefficient;

    std::string min_max = str(boost::format("%-i %.1f") % min % boost::io::group(std::setw(histogram_width), max));

    top_line << draw_line(labels_width, ' ', false) << " " <<  min_max;
    return top_line.str();
}

std::string draw_histogram(const std::vector<std::string>& labels, const std::vector<int>& values) {   
    const auto scale_factors = calculate_scale_factors(values);
    const auto labels_width = get_max_width(labels);
    
    std::stringstream visualisation;
    visualisation << "\n" << get_top_line(labels_width, values) << "\n";
    visualisation << get_external_line(labels_width) << "\n";
    
    for(int i = 0; i < values.size(); i++)
        visualisation << get_single_label(labels, i, labels_width) << " " << get_single_histogram_line(scale_factors, i) <<  "\n";
    
    visualisation << get_external_line(labels_width) << "\n\n";
    
    return visualisation.str();
}       

template <class histogram>
void display(const histogram& h) {
    std::vector<std::string> labels {};
    std::vector<int> values {};
    
    extract_data(h, labels, values);

    std::cout << draw_histogram(labels, values);
}


} // namespace display

#endif