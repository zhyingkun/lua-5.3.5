#! /usr/bin/env bash

Protocols=$(find ./proto -mindepth 1 -maxdepth 1 -type f -name "*\.proto")
# echo ${Protocols[@]}

for proto in ${Protocols[@]}; do
	name=${proto%\.proto}
	name=${name##*/}
	# echo ${name}
	if [ Y"${name}" = Y"desc_simple" ]; then
		echo "Do not compile simple descriptor.proto!"
	else
		echo "Compile:" ${proto}
		protoc ${proto} -o ./pb/${name}.pb
	fi
done
