#include <llvm/Support/ToolOutputFile.h>
#include "codegen.h"

namespace at {

void CodeGen::emitSignatureSection() {
    std::vector<BinSigDecl> decls = sigs_;
    if (!haveSigs_) {
        for (const auto& [name, shape] : templateShapes_) {
            BinSigDecl d;
            d.kind = BinSigDecl::Meta;
            d.name = name;
            for (unsigned i = 0; i < shape.second; ++i) d.templateParams.push_back("T" + std::to_string(i));
            for (const auto& pn : shape.first) d.params.push_back({pn, BinSigType::prim(BinSigType::PUnknown)});
            d.returnType = BinSigType::prim(BinSigType::PUnknown);
            decls.push_back(std::move(d));
        }
    }
    if (decls.empty()) return;

    std::vector<uint8_t> bytes = BinSigWriter().write(decls);
    auto* init = llvm::ConstantDataArray::get(context_, llvm::ArrayRef<uint8_t>(bytes));
    auto* gv = new llvm::GlobalVariable(*module_, init->getType(), true,
        llvm::GlobalValue::PrivateLinkage, init, "__at_sigs");
    gv->setSection(".at.sigs");
    gv->setAlignment(llvm::Align(1));
    // llvm::appendToCompilerUsed(*module_, {gv});  // optional
}

} // namespace at
