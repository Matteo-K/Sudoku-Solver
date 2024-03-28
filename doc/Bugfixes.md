# Bugfixes

## 1. Assertion failure (grid.c:130)

This assertion is a sanity check performed in `grid_provideValue`.
It asserts that the value is indeed possible (only considering values, not candidates) on the targeted cell before providing it, using the `grid_possible` macro.

This macro uses the `tGrid._is*Free` arrays internally as a performance optimization. Indeed, looking up and anding 3 booleans in arrays is orders of magnitude faster than going around the grid ensuring the value is indeed possible.

However, it means the state of the grid and the state of the value presence arrays could get desync, resulting in nasty, nasty bugs.

Solution : the problem came from the wrong index calculations performed in `at2d` and `at3d` which resulted in overlapping computed indexes for the same arguments, which lead to weird bugs.
