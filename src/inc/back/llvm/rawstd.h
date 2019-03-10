#ifndef PL01_BACK_LLVM_RAWSTD_H_
#define PL01_BACK_LLVM_RAWSTD_H_

#include <iostream>

#include <llvm/Support/raw_ostream.h>

// implement a subclass of llvm::raw_ostream
// to support 'print' to standard output stream (e.g. std::ofstream)
class RawStdOStream : public llvm::raw_ostream {
public:
    RawStdOStream() : os_(std::cerr) {}
    RawStdOStream(std::ostream &os) : os_(os) {}

private:
    void write_impl(const char *Ptr, size_t Size) override {
        os_.write(Ptr, Size);
    }

    uint64_t current_pos() const override {
        return os_.tellp();
    }

    std::ostream &os_;
};

#endif // PL01_BACK_LLVM_RAWSTD_H_
