# Backend Scoring System Design

> **Note**: For the complete Chinese documentation, see [中文版](../zh-CN/design/backend_scoring.md)

## Overview

One of VRHI's core innovations is its smart backend selection system. During initialization, VRHI detects all available backends, queries hardware-supported features, scores each backend based on application requirements, and selects the highest-scoring backend.

## Design Goals

1. **Automation**: No manual backend selection needed; system automatically chooses optimal solution
2. **Intelligence**: Considers features, performance, stability, and other factors
3. **Customizable**: Allows applications to specify required features and preferences
4. **Transparent**: Provides scoring details for debugging and optimization

## Scoring Dimensions

### 1. Feature Support Score (Weight: 40%)

Scores based on quantity and quality of features supported by the backend.

### 2. Performance Score (Weight: 30%)

Based on backend's theoretical performance and known benchmark results.

### 3. Stability Score (Weight: 20%)

Based on backend's maturity and known issues.

### 4. Compatibility Score (Weight: 10%)

Device and driver support for the backend.

## Scoring Algorithm

```
Total Score = (Feature Score × 0.4) + 
              (Performance Score × 0.3) + 
              (Stability Score × 0.2) + 
              (Compatibility Score × 0.1)
```

For detailed documentation including scoring flow, code examples, and platform-specific adjustments, please refer to the [Chinese version](../zh-CN/design/backend_scoring.md).
