**[English](./README.md)** | **[简体中文](./README.zh.md)**

---

# JSONpp (0.1.1)

A modern, header-only C++ JSON library based on generic programming.

> ⚠️ **Documentation Rewrite in Progress**
>
> Following the major v0.1.0 architectural overhaul (introducing `basic_json` template and IO abstraction), the previous documentation has been deprecated.
>
> We are currently rewriting the documentation to ensure accuracy. In the meantime, please refer to the **unit tests** in the `tests/` directory for up-to-date usage examples.

## Key Features
* **High Extensibility**: Header-only and template-based architecture allowing full customization of underlying containers (e.g., seamless switching between `std::map` and `std::unordered_map`) and types.
* **High Performance**: Specialized zero-copy parsing path optimized for `std::string_view` inputs.
* **Memory Efficiency**: Stream-based IO abstraction (`std::istream`) designed for handling large datasets with minimal memory footprint.
* **Generic Parsing**: Stream-based **Recursive Descent** parser engine supporting generic input sources and output types.