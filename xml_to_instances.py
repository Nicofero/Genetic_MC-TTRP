import xml.etree.ElementTree as ET

tree = ET.parse('instances/mc-vrp/CMT01.xml')
root = tree.getroot()

# Example: Print tag and attributes of each element
for elem in root.iter():
    print(f"Tag: {elem.tag}, Attributes: {elem.attrib}, Text: {elem.text.strip() if elem.text else ''}")