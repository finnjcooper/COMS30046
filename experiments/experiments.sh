make headless BENCH=dot CONFIG=experiments/configs/width_1.json
make headless BENCH=dot CONFIG=experiments/configs/width_2.json
make headless BENCH=dot CONFIG=experiments/configs/width_4.json
make headless BENCH=dot CONFIG=experiments/configs/width_8.json

make headless BENCH=fibonacci CONFIG=experiments/configs/width_1.json
make headless BENCH=fibonacci CONFIG=experiments/configs/width_2.json
make headless BENCH=fibonacci CONFIG=experiments/configs/width_4.json
make headless BENCH=fibonacci CONFIG=experiments/configs/width_8.json

make headless BENCH=gaussian CONFIG=experiments/configs/width_1.json
make headless BENCH=gaussian CONFIG=experiments/configs/width_2.json
make headless BENCH=gaussian CONFIG=experiments/configs/width_4.json
make headless BENCH=gaussian CONFIG=experiments/configs/width_8.json

make headless BENCH=insertionsort CONFIG=experiments/configs/width_1.json
make headless BENCH=insertionsort CONFIG=experiments/configs/width_2.json
make headless BENCH=insertionsort CONFIG=experiments/configs/width_4.json
make headless BENCH=insertionsort CONFIG=experiments/configs/width_8.json

make headless BENCH=matmul CONFIG=experiments/configs/width_1.json
make headless BENCH=matmul CONFIG=experiments/configs/width_2.json
make headless BENCH=matmul CONFIG=experiments/configs/width_4.json
make headless BENCH=matmul CONFIG=experiments/configs/width_8.json

make headless BENCH=quicksort CONFIG=experiments/configs/width_1.json
make headless BENCH=quicksort CONFIG=experiments/configs/width_2.json
make headless BENCH=quicksort CONFIG=experiments/configs/width_4.json
make headless BENCH=quicksort CONFIG=experiments/configs/width_8.json

make headless BENCH=saxpy CONFIG=experiments/configs/width_1.json
make headless BENCH=saxpy CONFIG=experiments/configs/width_2.json
make headless BENCH=saxpy CONFIG=experiments/configs/width_4.json
make headless BENCH=saxpy CONFIG=experiments/configs/width_8.json


make headless BENCH=dot_vec CONFIG=experiments/configs/vector_128.json
make headless BENCH=dot_vec CONFIG=experiments/configs/vector_256.json
make headless BENCH=dot_vec CONFIG=experiments/configs/vector_512.json

make headless BENCH=gaussian_vec CONFIG=experiments/configs/vector_128.json
make headless BENCH=gaussian_vec CONFIG=experiments/configs/vector_256.json
make headless BENCH=gaussian_vec CONFIG=experiments/configs/vector_512.json

make headless BENCH=matmul_vec CONFIG=experiments/configs/vector_128.json
make headless BENCH=matmul_vec CONFIG=experiments/configs/vector_256.json
make headless BENCH=matmul_vec CONFIG=experiments/configs/vector_512.json

make headless BENCH=saxpy_vec CONFIG=experiments/configs/vector_128.json
make headless BENCH=saxpy_vec CONFIG=experiments/configs/vector_256.json
make headless BENCH=saxpy_vec CONFIG=experiments/configs/vector_512.json


make headless BENCH=dot CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=dot CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=dot CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=dot CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=fibonacci CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=fibonacci CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=fibonacci CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=fibonacci CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=gaussian CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=gaussian CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=gaussian CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=gaussian CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=insertionsort CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=insertionsort CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=insertionsort CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=insertionsort CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=matmul CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=matmul CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=matmul CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=matmul CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=quicksort CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=quicksort CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=quicksort CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=quicksort CONFIG=experiments/configs/branch_two_bit.json

make headless BENCH=saxpy CONFIG=experiments/configs/branch_static_not_taken.json
make headless BENCH=saxpy CONFIG=experiments/configs/branch_static_taken.json
make headless BENCH=saxpy CONFIG=experiments/configs/branch_one_bit.json
make headless BENCH=saxpy CONFIG=experiments/configs/branch_two_bit.json