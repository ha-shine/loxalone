//
// Created by Htet Aung Shine on 24/11/22.
//
#include <fmt/format.h>
#include <sysexits.h>

#include <filesystem>
#include <fstream>
#include <vector>

#include "../Misc.h"

static const auto USAGE = "Usage: generate_ast <output directory>\n";

using namespace loxalone;

struct Field {
  std::string_view type, name;
};

struct Class {
  std::string_view name;
  std::vector<Field> fields;
};

auto define_type(std::ostream& out, const std::string_view& base,
                 const Class& cls) -> void {
  out << fmt::format("class {}", cls.name) << " {\n"
      << " public:\n";
  for (const auto& field : cls.fields) {
    // Not very great but this will do for now...
    out << fmt::format("  const {} {}_m;\n", field.type, field.name);
  }

  out << "\n"
      << (cls.fields.size() == 1 ? "  explicit " : "  ") << cls.name << "(";

  for (int i = 0; i < cls.fields.size(); i++) {
    out << fmt::format("{}&& {}", cls.fields[i].type, cls.fields[i].name);
    if (i < cls.fields.size() - 1) out << ", ";
  }

  out << "): ";

  // TODO: Figure out the escape character for braces in fmt
  for (int i = 0; i < cls.fields.size(); i++) {
    out << cls.fields[i].name << "_m{std::move(" << cls.fields[i].name << ")}";
    if (i < cls.fields.size() - 1) out << ", ";
  }

  out << " {}\n" << fmt::format("  ~{}() = default;\n\n", cls.name);

  // helper method for creating this type
  out << "  static auto create(";
  for (int i = 0; i < cls.fields.size(); i++) {
    out << fmt::format("{}&& {}", cls.fields[i].type,
                       to_lowercase(cls.fields[i].name));
    if (i < cls.fields.size() - 1) out << ", ";
  }
  out << fmt::format(") -> {}Ptr {{\n", cls.name)
      << fmt::format("    return std::make_unique<{}>(", cls.name);
  for (int i = 0; i < cls.fields.size(); i++) {
    out << fmt::format("std::move({})", cls.fields[i].name);
    if (i < cls.fields.size() - 1) out << ", ";
  }
  out << ");\n"
      << "  }\n\n";

  // helper method for creating empty() method for creating empty pointer of
  // this type
  out << fmt::format("  static auto empty() -> {} {{\n", base)
      << fmt::format("    return std::unique_ptr<{}>(nullptr);\n", cls.name)
      << "  }\n\n";

  // end
  out << "};\n\n";
}

// TODO: Revisit this by checking out other interpreters, how do we implement
//       visiting without pointers?
// TODO: Maybe a better way is to have a template engine do all of these for us?
auto define_ast(const std::filesystem::path& filepath,
                const std::string_view& base,
                const std::vector<std::string_view>& types,
                const std::vector<std::string_view>& imports) -> int {
  std::ofstream out{filepath, std::ios_base::out};

  out << fmt::format("#ifndef LOXALONE_{}_H\n", base)
      << fmt::format("#define LOXALONE_{}_H\n\n", base) << "#include <memory>\n"
      << "#include <vector>\n"
      << "#include <string>\n"
      << "#include <variant>\n\n"
      << "#include \"Token.h\"\n\n";

  for (const auto& imp : imports) {
    out << fmt::format("#include \"{}\"\n", imp);
  }
  out << '\n';

  out << "namespace loxalone {\n\n";
  std::vector<Class> classes{};
  for (const auto& type : types) {
    std::vector parts = split(type, '-');
    std::string_view name = trim(parts[0]), fields_string = trim(parts[1]);

    std::vector<Field> fields{};
    for (const auto& part : split(fields_string, ',')) {
      std::vector<std::string_view> field_parts = split(trim(part), ' ');
      fields.emplace_back(Field{field_parts[0], field_parts[1]});
    }

    classes.emplace_back(Class{name, fields});
  }

  for (const auto& cls : classes) {
    out << fmt::format("class {};\n", cls.name);
  }
  out << "\n";

  // define pointer types
  for (const auto& cls : classes) {
    out << fmt::format("using {}Ptr = std::unique_ptr<{}>;\n", cls.name,
                       cls.name);
  }
  out << "\n";

  // define Expr type
  out << fmt::format("using {} = std::variant<", base);
  for (int i = 0; i < classes.size(); i++) {
    out << classes[i].name << "Ptr";
    if (i < classes.size() - 1) out << ",";
  }
  out << ">;\n\n";

  // define base_is_null function for pointers
  out << fmt::format("static auto {}_is_null(const {}& {}) {{\n",
                     to_lowercase(base), base, to_lowercase(base))
      << fmt::format(
             "  return visit([](auto&& arg) -> bool {{ return arg == nullptr; "
             "}}, {});\n",
             to_lowercase(base))
      << fmt::format("}}\n\n");

  // define Is<Type> concept
  out << fmt::format("template <typename T>\n")
      << fmt::format("concept Is{} =", base);
  for (int i = 0; i < classes.size(); i++) {
    out << fmt::format(" std::same_as<T, {}Ptr>", classes[i].name);
    if (i < classes.size() - 1) out << " ||";
  }
  out << ";\n\n";

  // define Visitor concept so callers can static_assert their type to ensure
  // the visitor classes are compliant, ie define all required methods
  out << "template <typename V, typename Out>\n"
      << fmt::format("concept {}Visitor = requires (V v,", base);
  for (int i = 0; i < classes.size(); i++) {
    out << fmt::format(" const {}Ptr& arg_{}", classes[i].name, i);
    if (i < classes.size() - 1) out << ",";
  }
  out << ") { \n";
  for (int i = 0; i < classes.size(); i++) {
    out << fmt::format("  {{ v(arg_{}) }} -> std::convertible_to<Out>;\n", i);
  }
  out << "};\n\n";

  for (const auto& cls : classes) {
    define_type(out, base, cls);
  }
  out << "}\n\n"; // end of namespace

  out << "\n#endif\n";

  return 0;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print(stderr, USAGE);
    return EX_USAGE;
  }

  auto filepath = std::filesystem::path{argv[1]};
  std::filesystem::create_directories(filepath);

  // clang-format off
  define_ast(filepath / "Expr.h", "Expr", {
              "Assign   - Token name, Expr value",
              "Binary   - Expr left, Token oper, Expr right",
              "Call     - Expr callee, Token paren, std::vector<Expr> arguments",
              "Grouping - Expr expression",
              "Literal  - lox_literal value",
              "Logical  - Expr left, Token oper, Expr right",
              "Unary    - Token oper, Expr right",
              "Variable - Token name"}, {});

  define_ast(filepath / "Stmt.h", "Stmt", {
              "Block      - std::vector<Stmt> statements",
              "Expression - Expr expression",
              "Function   - Token name, std::vector<Token> params, std::vector<Stmt> body",
              "If         - Expr expression, Token token, Stmt then_branch, Stmt else_branch",
              "While      - Expr condition, Stmt body, Token token",
              "Print      - Expr expression",
              "Return     - Token keyword, Expr value",
              "Var        - Token name, Expr initializer"}, {"Expr.h"});
  // clang-format on
}