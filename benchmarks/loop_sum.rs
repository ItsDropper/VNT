use std::hint::black_box;

fn main() {
    let mut round: i64 = 0;
    let mut i: i64 = 0;
    let mut total: i64 = 0;

    while round < 20 {
        i = 0;
        total = 0;
        while i < 40_000 {
            total += i;
            i += 1;
            black_box((i, total));
        }
        round += 1;
    }

    println!("{total}");
}
