#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {
}

void Sheet::SetCell(Position pos, std::string text) {
if(!pos.IsValid()) {
throw InvalidPositionException("invalid position");
}
auto& cell = cells_[pos];
if(!cell) {
    cell = std::make_unique<Cell>(*this);
}
cell->Set(std::move(text));

}
const CellInterface* Sheet::GetCell(Position pos) const {
if (!pos.IsValid()) {
throw InvalidPositionException("invalid position");
}
auto it = cells_.find(pos);
if (it == cells_.end()) {
return nullptr;
}
return it->second.get();
}

CellInterface* Sheet::GetCell(Position pos) {
return const_cast<CellInterface*>(static_cast<const Sheet&>(*this).GetCell(pos));
}
  

void Sheet::ClearCell(Position pos) {
if (!pos.IsValid()) {
throw InvalidPositionException("invalid position");
}
auto it = cells_.find(pos);
if (it != cells_.end()) {
it->second->Clear();
cells_.erase(it);
}
}

Size Sheet::GetPrintableSize() const {
Size result{ 0, 0 };
for (const auto& [pos, cell] : cells_) {
result.rows = std::max(result.rows, pos.row + 1);
result.cols = std::max(result.cols, pos.col + 1);
}
return result;
}

void Sheet::PrintValues(std::ostream& output) const {
Size size = GetPrintableSize();
for (int row = 0; row < size.rows; ++row) {
for (int col = 0; col < size.cols; ++col) {
if (col > 0) {
output << "\t";
}
const auto* cell = GetCell({ row, col });
if (cell && !cell->GetText().empty()) {
std::visit([&](const auto& value) { output << value; }, cell->GetValue());
}
}
output << "\n";
}
}


void Sheet::PrintTexts(std::ostream& output) const {
Size size = GetPrintableSize();
for (int row = 0; row < size.rows; ++row) {
for (int col = 0; col < size.cols; ++col) {
if (col > 0) {
output << "\t";
}
const auto* cell = GetCell({ row, col });
if (cell && !cell->GetText().empty()) {
output << cell->GetText();
}
}
output << "\n";
}
}

const Cell* Sheet::GetCellPtr(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    const auto cell = cells_.find(pos);
    
    if (cell == cells_.end()) {
        return nullptr;
    }

    return cells_.at(pos).get();
}

Cell* Sheet::GetCellPtr(Position pos) {
    return const_cast<Cell*>(
        static_cast<const Sheet&>(*this).GetCellPtr(pos));
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}
