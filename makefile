default:
	gcc -o src/parse src/parse.c src/tokenizer.c src/json.c src/string.c src/vec.c src/iterator.c
	./src/parse
