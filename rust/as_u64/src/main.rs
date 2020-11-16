fn main() {
    let x = std::u64::MAX as f64;
    let xplus1 = x + 1.0;
    dbg!((std::u64::MAX, x, x as u64, xplus1, xplus1 as u64));

    let x = std::u128::MAX as f64;
    let xplus1 = x + 1.0;
    dbg!((std::u128::MAX, x, x as u128, xplus1, xplus1 as u128));
}
