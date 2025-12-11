#ifndef STANZA_HPP
#define STANZA_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

/* DFV: Data-File and Vector*/
namespace dfv
{
  template <typename T>

    /** LoadTxtCol:
     *
     * Reads a specific column from a text file into a vector, skipping initial rows.
     * The function assumes the columns in the file are separated by whitespace.
     *
     */
    std::vector<T> 
    LoadTxtCol(
        const std::string& file_path, 
        int column_index, 
        int skip_rows = 0)
    {

      std::vector<T> column_data;
      std::ifstream file(file_path);

      if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << file_path << std::endl;
        return column_data;
      }

      std::string line;
      int current_line_count = 0;

      // Skip input number of rows
      while (current_line_count < skip_rows && std::getline(file, line)) {
        current_line_count++;
      }
      if (current_line_count < skip_rows) {
        std::cerr << "Warning: Reached end of file before skipping " << skip_rows 
          << " rows. Only skipped " << current_line_count << " rows." << std::endl;
      }

      // Read data
      while (std::getline(file, line)) 
      {
        std::stringstream ss(line);
        std::string token;
        int current_column = 0;
        bool found = false;

        while (ss >> token) 
        {
          if (current_column == column_index) 
          {
            T value;
            std::stringstream convert_ss(token);
            if (convert_ss >> value)
            {
              column_data.push_back(value);
              found = true;
            } else {
              std::cerr << "Warning: Data conversion failed for value '" << token 
                << "' in column " << column_index
                << ". Skipping value and line." << std::endl;
            }
            break;
          }
          current_column++;
        }
      
        if (!found && current_column <= column_index) 
        {
          std::cerr << "Column index not found" 
            << column_index << ". Skipping this line." << std::endl;
        }
      }
      return column_data;
    }


  //*************************************************************************
  //*************************************************************************


  /** SaveTxt:
   *
   * Writes multiple vectors as columns to a text file, prefixed by a user-defined header row.
   *
   * Each element at index 'i' from all input vectors forms a single row. The provided
   * 'column_headers' vector is written as the first row.
   *
   */
  template <typename T>
    bool SaveTxt(
        const std::string& file_path,
        const std::vector<std::vector<T>>& vectors,
        const std::vector<std::string>& column_headers,
        const std::string& delimiter = " ",
        int precision = 6)
    {
      if (vectors.empty()) {
        if (!column_headers.empty()) {
        } else {
          return true; 
        }
      }
      const size_t num_cols = vectors.empty() ? column_headers.size() : vectors.size();

      if (column_headers.size() != num_cols) {
        throw std::length_error("Number of column headers must match number of columns.");
      }

      size_t num_rows = 0;
      if (!vectors.empty()) {
        num_rows = vectors[0].size();
        for (const auto& vec : vectors) {
          if (vec.size() != num_rows) {
            throw std::length_error("Input data vectors must have the same size.");
          }
        }
      }

      std::ofstream outfile(file_path);
      if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file " << file_path  << "\n";
        return false;
      }

      // Headers
      for (size_t j = 0; j < num_cols; ++j) {
        outfile << column_headers[j];
        if (j < num_cols - 1) {
          outfile << delimiter;
        }
      }
      outfile << "\n";

      // Set precision for data rows
      outfile << std::fixed << std::setprecision(precision);
      
      // Write stuff
      for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_cols; ++j) {
          outfile << vectors[j][i];
          if (j < num_cols - 1) {
            outfile << delimiter;
          }
        }
        outfile << "\n";
      }

      return true;
    }


} // end dfv 


#endif // READFILE_HPP
