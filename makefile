build: src/json_parser.c
	gcc src/json_parser.c src/json_tokenization.c src/json_abstraction.c src/gen_vec.c -o src/json_parser

run: build
	./src/json_parser
	
