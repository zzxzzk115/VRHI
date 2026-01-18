# Feature Detection Example

This example demonstrates VRHI's feature detection system.

## What it does

- Enumerates all available backends
- Displays comprehensive feature support information for each backend
- Tests feature requirement validation
- Demonstrates how to query individual features

## Building

```bash
cmake --build build --target FeatureDetection
```

## Running

```bash
./build/bin/examples/FeatureDetection
```

## Expected Output

The example will print:
- List of available backends with their scores
- Detailed feature capabilities for each backend
- Results of feature requirement validation tests

This demonstrates the Feature Detection functionality described in KANBAN tasks B2.1 and B2.2.
