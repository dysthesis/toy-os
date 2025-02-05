#pragma once

// Return type for SBI values.
// NOTE: why could just be a tagged union ?
struct sbiret {
  long error;
  long value;
};
