default:
	gcc -o src/main src/main.c src/beautify.c src/parse.c src/tokenizer.c src/json.c src/string.c src/vec.c src/iterator.c
	./src/main
