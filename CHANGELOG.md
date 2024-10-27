# Changelog

## 0.1.11 (2024-10-27)

Fix
- Fix incorrect type casting when parsing a number

## 0.1.10 (2024-10-23)

Features
- Support deletion of entries from JSON node of object or array type.

## 0.1.9 (2024-10-19)

CI
- Add code summary (code lines count)

## 0.1.8 (2024-10-17)

Test
- Add parsing tests for randomly generated large-json data

Refactor
- Refactor parser code

## 0.1.7 (2024-10-15)

Features
- Support iterating over object and array JSON data types

## 0.1.6 (2024-10-12)

Features
- Support initializing object and array using initializer list

## 0.1.5 (2024-10-09)

Features
- Support modifying value stored at the JSON node
- Support creating JSON node implicitly from value

CI
- disable explicit constructor check

## 0.1.4 (2024-10-07)

Features
- Support getting value stored at the JSON node

## 0.1.3 (2024-10-06)

Features
- Support number type JSON parsing

## 0.1.2 (2024-10-05)

Features
- Support object type JSON parsing

## 0.1.1 (2024-10-04)

Features
- Support basic string parsing
- Throw error when JSON string is invalid

## 0.1.0 (2024-10-03)

Features
- Support parsing for boolean, null, and array

CI
- Add linting, functional tests, memory leak check, and to Github CI

Tests
- Add basics tests for parsing boolean, null, and array
