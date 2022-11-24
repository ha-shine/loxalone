//
// Created by Htet Aung Shine on 24/11/22.
//
#include <fmt/format.h>
#include <sysexits.h>
#include <filesystem>
#include <fstream>
#include <vector>

#include "../misc.h"

static const auto USAGE = "Usage: generate_ast <output directory>\n";

struct ClsField {
  std::string_view type, name;
};

auto define_type(std::ostream& out, const std::string_view& base,
                 const std::string_view& cls,
                 const std::vector<std::string_view>& fields) -> void {
  std::vector<ClsField> clsfields{};
  for (const auto& field : fields) {
    std::vector parts = split(field, ' ');
    clsfields.emplace_back(ClsField{trim(parts[0]), trim(parts[1])});
  }

  out << fmt::format("class {} : {}", cls, base) << " {\n"
      << " public:\n"
      << (fields.size() == 1 ? "  explicit " : "  ") << cls << "(";

  for (int i = 0; i < clsfields.size(); i++) {
    out << fmt::format("{}&& {}", clsfields[i].type, clsfields[i].name);
    if (i < clsfields.size() - 1)
      out << ", ";
  }

  out << "): ";

  // TODO: Figure out the escape character for braces in fmt
  for (int i = 0; i < clsfields.size(); i++) {
    out << clsfields[i].name << "_m{" << clsfields[i].name << "}";
    if (i < clsfields.size() - 1)
      out << ", ";
  }

  out << " {}\n"
      << fmt::format("  ~{}() override = default;\n\n", cls)
      << " private:\n";

  for (const auto& field : clsfields) {
    out << fmt::format("  const {} {}_m;\n", field.type, field.name);
  }
  out << "};\n\n";
}

auto define_ast(std::ostream& out, const std::string_view& base,
                const std::vector<std::string_view>& types) -> int {
  out << "#ifndef LOXALONE_EXPR_H\n"
      << "#define LOXALONE_EXPR_H\n\n"
      << "#include <string>\n"
      << "#include <variant>\n"
      << "#include \"tokens.h\"\n\n"
      << "class Expr {\n"
      << " public:\n"
      << "  virtual ~Expr() = default;\n"
      << "};\n\n";

  for (const auto& type : types) {
    std::vector parts = split(type, ':');
    std::string_view name = trim(parts[0]), fields_string = trim(parts[1]);

    std::vector<std::string_view> fields{};
    for (const auto& part : split(fields_string, ','))
      fields.emplace_back(trim(part));

    define_type(out, base, name, fields);
  }

  // Here we will output each classes
  out << "\n#endif\n";

  return 0;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print(stderr, USAGE);
    return EX_USAGE;
  }

  auto filepath = std::filesystem::path{argv[1]} / "expr.h";
  std::filesystem::create_directories(filepath.parent_path());

  std::ofstream out{filepath, std::ios_base::out};
  return define_ast(out, "Expr",
                    {"BinaryExpr   : Expr left, Token oper, Expr right",
                     "GroupingExpr : Expr expression",
                     "LiteralVal   : lox_literal value",
                     "UnaryExpr    : Token oper, Expr right"});
}