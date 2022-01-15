# Leka - LekaOS
# Copyright 2021 APF France handicap
# SPDX-License-Identifier: Apache-2.0

shopt -s xpg_echo

BASE_DIR=$1
HEAD_DIR=$2

source ./.github/actions/compare_files/get_all_targets.sh $BASE_DIR $HEAD_DIR

echo 'STATUS_DIFF_OUTPUT<<EOF_STATUS_DIFF_OUTPUT' >> $GITHUB_ENV

echo "| Target | Status | .bin | .map | Total Flash (base/head) | Total Flash Δ | Static RAM (base/head) | Static RAM Δ |" >> $GITHUB_ENV
echo "|-------|:------:|:------:|:------:|:------:|:------:|:------:|:------:|" >> $GITHUB_ENV

for target in "${all_targets[@]}"; do
	target_name=$target

	echo -n "| $target_name " >> $GITHUB_ENV

	if [[ " ${added_targets[*]} " =~ " $target " ]]; then

		echo -n "| :sparkles: | - | - " >> $GITHUB_ENV

		createSizeTextFile $HEAD_DIR $target_name

		head_flash_with_percentage="$(getUsedFlashSizeWithPercentage $HEAD_DIR $target_name)"
		head_ram_with_percentage="$(getUsedRamSizeWithPercentage $HEAD_DIR $target_name)"

		echo -n "| $head_flash_with_percentage | - | $head_ram_with_percentage | - |\n" >> $GITHUB_ENV

	elif [[ " ${deleted_targets[*]} " =~ " $target " ]]; then

		echo -n "| :coffin: | - | - | - | - | - | - |\n" >> $GITHUB_ENV

	else

		echo -n "| :heavy_check_mark: " >> $GITHUB_ENV

		if ! output=$(diff $HEAD_DIR/$target_name.bin $HEAD_DIR/$target_name.bin 2>/dev/null); then
			echo -n "| :x: " >> $GITHUB_ENV
		else
			echo -n "| :white_check_mark: " >> $GITHUB_ENV
		fi

		createMapTextFile $BASE_DIR $target_name
		createMapTextFile $HEAD_DIR $target_name

		createSizeTextFile $BASE_DIR $target_name
		createSizeTextFile $HEAD_DIR $target_name


		if ! output=$(diff $BASE_DIR/$target_name-map.txt $HEAD_DIR/$target_name-map.txt 2>/dev/null); then
			echo -n "| :x: " >> $GITHUB_ENV
		else
			echo -n "| :white_check_mark: " >> $GITHUB_ENV
		fi

		base_flash_with_percentage="$(getUsedFlashSizeWithPercentage $BASE_DIR $target_name)"
		head_flash_with_percentage="$(getUsedFlashSizeWithPercentage $HEAD_DIR $target_name)"

		base_flash="$(getUsedFlashSize $BASE_DIR $target_name)"
		head_flash="$(getUsedFlashSize $HEAD_DIR $target_name)"

		diff_flash=$(($head_flash - $base_flash))
		diff_flash_percentage="$((100 * ($head_flash - $base_flash) / $base_flash))%"

		output_flash="$base_flash_with_percentage<br>$head_flash_with_percentage"
		output_flash_delta=""

		if [ $diff_flash -lt 0 ]; then
			output_flash_delta=":chart_with_downwards_trend:<br>$diff_flash&nbsp;($diff_flash_percentage)"
		elif [ $diff_flash -gt 0 ]; then
			output_flash_delta=":chart_with_upwards_trend:<br>$diff_flash&nbsp;($diff_flash_percentage)"
		else
			output_flash="$base_flash_with_percentage"
			output_flash_delta="ø"
		fi

		base_ram_with_percentage="$(getUsedRamSizeWithPercentage $BASE_DIR $target_name)"
		head_ram_with_percentage="$(getUsedRamSizeWithPercentage $HEAD_DIR $target_name)"

		base_ram_percentage=$(grep -Po '(?<=SRAM used:\s)[[:digit:]]*\s\([[:digit:]]*%\)' $BASE_DIR/$target_name-code_size.txt)
		head_ram_percentage=$(grep -Po '(?<=SRAM used:\s)[[:digit:]]*\s\([[:digit:]]*%\)' $HEAD_DIR/$target_name-code_size.txt)

		base_ram="$(getUsedRamSize $BASE_DIR $target_name)"
		head_ram="$(getUsedRamSize $HEAD_DIR $target_name)"

		diff_ram=$(($head_ram - $base_ram))
		diff_ram_percentage="$((100 * ($head_ram - $base_ram) / $base_ram))%"

		output_ram="$base_ram_with_percentage<br>$head_ram_with_percentage"
		output_ram_delta=""

		if [ $diff_ram -lt 0 ]; then
			output_ram_delta=":chart_with_downwards_trend:<br>$diff_ram&nbsp;($diff_ram_percentage)"
		elif [ $diff_ram -gt 0 ]; then
			output_ram_delta=":chart_with_upwards_trend:<br>$diff_ram&nbsp;($diff_ram_percentage)"
		else
			output_ram="$base_ram_with_percentage"
			output_ram_delta="ø"
		fi

		echo -n "| $output_flash | $output_flash_delta | $output_ram | $output_ram_delta " >> $GITHUB_ENV

		echo -n "|\n" >> $GITHUB_ENV
	fi

done

echo 'EOF_STATUS_DIFF_OUTPUT' >> $GITHUB_ENV
