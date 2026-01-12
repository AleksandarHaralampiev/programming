#include <stdio.h>
#include <stdbool.h>

// Основна NOR функция: ¬(x ∨ y)
bool nor(bool x, bool y) {
    return !(x || y);
}

// NOT функция: ¬x
// ¬x = ¬(x ∨ x) = NOR(x, x)
bool not(bool x) {
    return nor(x, x);
}

// AND функция: x ∧ y
// x ∧ y = ¬(¬x ∨ ¬y) = ¬(¬x ∨ ¬y) = NOR(NOR(x,x), NOR(y,y))
bool and(bool x, bool y) {
    return nor(nor(x, x), nor(y, y));
}

// OR функция: x ∨ y
// x ∨ y = ¬¬(x ∨ y) = ¬(¬(x ∨ y)) = NOT(NOR(x, y))
bool or(bool x, bool y) {
    return nor(nor(x, y), nor(x, y));
}

// IMPLY функция: x → y
// x → y = ¬x ∨ y = NOR(NOR(NOR(x,x), NOR(x,x)), NOR(y,y))
// Или по-просто: x → y = ¬x ∨ y
bool imply(bool x, bool y) {
    bool not_x = nor(x, x);
    return nor(nor(not_x, y), nor(not_x, y));
}

// EQUIV функция: x ↔ y
// x ↔ y = (x → y) ∧ (y → x) = (¬x ∨ y) ∧ (¬y ∨ x)
bool equiv(bool x, bool y) {
    bool impl_xy = imply(x, y);
    bool impl_yx = imply(y, x);
    return and(impl_xy, impl_yx);
}

// Алтернативна имплементация на EQUIV използвайки факта че:
// x ↔ y = (x ∧ y) ∨ (¬x ∧ ¬y)
bool equiv_alt(bool x, bool y) {
    bool both_true = and(x, y);
    bool both_false = and(not(x), not(y));
    return or(both_true, both_false);
}

