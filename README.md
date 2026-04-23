Here’s the updated minimal README with compile command:

---

# CheckMate

## Overview

CheckMate is a C-based manufacturing quality control system that manages components through production, inspection, and reporting. It simulates an industrial workflow using CSV-based data storage.

## Features

* Define components with multiple parameters and tolerances
* Track status: PENDING, APPROVED, REJECTED
* Perform quality inspection with real-time validation
* Maintain timeline logs of inspection results
* Generate reports for all components
* Reprocess rejected components

## Workflow

Components are created in the production module with status set to PENDING.
The quality module inspects each parameter against expected values and tolerances.

If all parameters pass, the component is marked APPROVED.
If any parameter fails, the component is immediately marked REJECTED and logged.

Rejected components can be corrected in production and reset to PENDING, allowing them to go through inspection again.

## Tech Stack

* C programming language
* CSV files for data storage
* Modular structure with file handling and enums

## Structure

* home.c: entry point and login
* production.c: component creation and management
* quality.c: inspection logic
* admin.c: reports and timeline

## Compilation

gcc home.c -o checkmate

## Usage

./checkmate

## Notes

The system focuses on simulating a real-world quality control loop including rejection, correction, and reinspection.
