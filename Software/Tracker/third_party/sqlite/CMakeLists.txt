cmake_minimum_required(VERSION 3.1)
project(sqlite)

add_library(sqlite
  sqlite3/sqlite3.c
)
