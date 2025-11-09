cd ~/ACA/simplescalar/simplesim-3.0

#./sim-cache -config ~/ACA/cacheconfig.txt ~/ACA/simplescalar/simplesim-3.0/tests-pisa/bin.little/test-math
#./sim-cache -config ~/ACA/cacheconfig.txt ../spec95-little/vortex.ss ~/ACA/inputs/vortex.in > ~/ACA/results/vortex_cache_output.txt

./sim-cache -prefetcher next_line -config ~/ACA/cacheconfig.txt ~/ACA/simplescalar/simplesim-3.0/tests-pisa/bin.little/test-math > result_nextline.txt
./sim-cache -prefetcher stride -config ~/ACA/cacheconfig.txt ~/ACA/simplescalar/simplesim-3.0/tests-pisa/bin.little/test-math > result_stride.txt
