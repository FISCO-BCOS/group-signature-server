find . -name "*.json" | while read LINE; do dos2unix $LINE; done
find . -name "*.sh" | while read LINE; do chmod +x ${LINE};dos2unix ${LINE}; done
