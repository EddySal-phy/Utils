#ifndef STANZA_HPP
#define STANZA_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>


/* DFV: Data-File and Vector*/
namespace dfv
{
  template <typename T>

    /* LoadTxtCol:
     *
     * Reads a specific column from a text file into a vector, skipping initial rows.
     * The function assumes the columns in the file are separated by whitespace.
     *
     *****************************************
     * Example:
     *
     * #include "stanza.hpp"
     *
     * int main() {
     *  const std::string filename = "data.txt";
     *  std::vector<double> col0 = dfv::LoadTxtCol<double>(filename, 1, 3);
     *
     *  std::cout << "--- (Column 1)  (Skipped first 3 rows) ---" << std::endl;
     *  for (const auto& val : col0)
     *    std::cout << val << std::endl;
     * 
     *  return 0;
     * }
     *
     ************************************************************
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

} // end dfv 


#endif // READFILE_HPP
