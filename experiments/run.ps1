make headless BENCH=dot CONFIG=experiments/width/1.json
make headless BENCH=dot CONFIG=experiments/width/2.json
make headless BENCH=dot CONFIG=experiments/width/4.json
make headless BENCH=dot CONFIG=experiments/width/8.json

make headless BENCH=fibonacci CONFIG=experiments/width/1.json
make headless BENCH=fibonacci CONFIG=experiments/width/2.json
make headless BENCH=fibonacci CONFIG=experiments/width/4.json
make headless BENCH=fibonacci CONFIG=experiments/width/8.json

make headless BENCH=gaussian CONFIG=experiments/width/1.json
make headless BENCH=gaussian CONFIG=experiments/width/2.json
make headless BENCH=gaussian CONFIG=experiments/width/4.json
make headless BENCH=gaussian CONFIG=experiments/width/8.json

make headless BENCH=insertionsort CONFIG=experiments/width/1.json
make headless BENCH=insertionsort CONFIG=experiments/width/2.json
make headless BENCH=insertionsort CONFIG=experiments/width/4.json
make headless BENCH=insertionsort CONFIG=experiments/width/8.json

make headless BENCH=matmul CONFIG=experiments/width/1.json
make headless BENCH=matmul CONFIG=experiments/width/2.json
make headless BENCH=matmul CONFIG=experiments/width/4.json
make headless BENCH=matmul CONFIG=experiments/width/8.json

make headless BENCH=quicksort CONFIG=experiments/width/1.json
make headless BENCH=quicksort CONFIG=experiments/width/2.json
make headless BENCH=quicksort CONFIG=experiments/width/4.json
make headless BENCH=quicksort CONFIG=experiments/width/8.json

make headless BENCH=saxpy CONFIG=experiments/width/1.json
make headless BENCH=saxpy CONFIG=experiments/width/2.json
make headless BENCH=saxpy CONFIG=experiments/width/4.json
make headless BENCH=saxpy CONFIG=experiments/width/8.json


make headless BENCH=dot CONFIG=experiments/vector/baseline.json
make headless BENCH=dot_vec CONFIG=experiments/vector/128.json
make headless BENCH=dot_vec CONFIG=experiments/vector/256.json
make headless BENCH=dot_vec CONFIG=experiments/vector/512.json

make headless BENCH=gaussian CONFIG=experiments/vector/baseline.json
make headless BENCH=gaussian_vec CONFIG=experiments/vector/128.json
make headless BENCH=gaussian_vec CONFIG=experiments/vector/256.json
make headless BENCH=gaussian_vec CONFIG=experiments/vector/512.json

make headless BENCH=matmul CONFIG=experiments/vector/baseline.json
make headless BENCH=matmul_vec CONFIG=experiments/vector/128.json
make headless BENCH=matmul_vec CONFIG=experiments/vector/256.json
make headless BENCH=matmul_vec CONFIG=experiments/vector/512.json

make headless BENCH=saxpy CONFIG=experiments/vector/baseline.json
make headless BENCH=saxpy_vec CONFIG=experiments/vector/128.json
make headless BENCH=saxpy_vec CONFIG=experiments/vector/256.json
make headless BENCH=saxpy_vec CONFIG=experiments/vector/512.json


make headless BENCH=dot CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=dot CONFIG=experiments/branch/static_taken.json
make headless BENCH=dot CONFIG=experiments/branch/btfnt.json
make headless BENCH=dot CONFIG=experiments/branch/two_bit.json

make headless BENCH=fibonacci CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=fibonacci CONFIG=experiments/branch/static_taken.json
make headless BENCH=fibonacci CONFIG=experiments/branch/btfnt.json
make headless BENCH=fibonacci CONFIG=experiments/branch/two_bit.json

make headless BENCH=gaussian CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=gaussian CONFIG=experiments/branch/static_taken.json
make headless BENCH=gaussian CONFIG=experiments/branch/btfnt.json
make headless BENCH=gaussian CONFIG=experiments/branch/two_bit.json

make headless BENCH=insertionsort CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=insertionsort CONFIG=experiments/branch/static_taken.json
make headless BENCH=insertionsort CONFIG=experiments/branch/btfnt.json
make headless BENCH=insertionsort CONFIG=experiments/branch/two_bit.json

make headless BENCH=matmul CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=matmul CONFIG=experiments/branch/static_taken.json
make headless BENCH=matmul CONFIG=experiments/branch/btfnt.json
make headless BENCH=matmul CONFIG=experiments/branch/two_bit.json

make headless BENCH=quicksort CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=quicksort CONFIG=experiments/branch/static_taken.json
make headless BENCH=quicksort CONFIG=experiments/branch/btfnt.json
make headless BENCH=quicksort CONFIG=experiments/branch/two_bit.json

make headless BENCH=saxpy CONFIG=experiments/branch/static_not_taken.json
make headless BENCH=saxpy CONFIG=experiments/branch/static_taken.json
make headless BENCH=saxpy CONFIG=experiments/branch/btfnt.json
make headless BENCH=saxpy CONFIG=experiments/branch/two_bit.json