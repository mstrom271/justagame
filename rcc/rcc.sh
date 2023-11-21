#!/bin/bash
##### the script will create rcc.qrc, listing all files in rcc folder #####


cd $(dirname "$0")

resources_dir="rcc"
output_file="rcc.qrc"

echo "<RCC>" > "$output_file"
echo "  <qresource prefix=\"/\">" >> "$output_file"

find "$resources_dir" -type f | while read -r file; do
    # get relative filepath
    resource_path="${file#$resources_dir}"
    # delete leading slash
    resource_path="${resource_path#/}"
    # XML shielding
    resource_path="${resource_path//&/&amp;}"
    resource_path="${resource_path//</&lt;}"
    resource_path="${resource_path//>/&gt;}"

    echo "    <file>rcc/$resource_path</file>" >> "$output_file"
done

echo "  </qresource>" >> "$output_file"
echo "</RCC>" >> "$output_file"

echo "Файл $output_file создан."
