# Build Help

```bash
Usage: make <TARGET> [OPTION=]...
Targets:
    help: Show build usage information (current file).
    check: Show build parameters.
    clean: Clean last built files.
    build: Build the project.
    rebuild: Force clean last built files and build again.
Options:
    TEST: Build a Test Application (add TEST global Define to code).
```

# Build Examples

```bash
make help
make check
make clean
make build
make build TEST=1
make rebuild
```
