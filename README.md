# Sudoku Generation using GAlib

## Overview

This project implements a Sudoku puzzle generator using Genetic Algorithms (GA). The solution is developed using GAlib, a C++ library for genetic algorithms.

## Features

- Generates valid Sudoku puzzles
- Uses GAlib for the implementation of the genetic algorithm
- Dockerized for easy setup and deployment

## Directory Structure

- `Dockerfile`: Configuration file for Docker
- `Makefile`: Build script
- `sudoku.cpp`: Main source code for the Sudoku generator
- `database/`: Database connection related files
  - `db_connector.cpp`
  - `db_connector.h`

## Usage

I have used this project to generate daily sudokus. For this, an additional docker-compose file is necessary, in which the database environment variables are declared.
To run this daily I have used Ofelia.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

## Acknowledgments

- [GAlib](https://web.mit.edu/galib/www/GAlib.html) - Matthew Wall's Genetic Algorithm Library
