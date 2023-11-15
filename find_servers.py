from bs4 import BeautifulSoup
import requests

def scrape_website(url):
    response = requests.get(url)
    soup = BeautifulSoup(response.text, 'html.parser')
    usable_computers = []
    high_load_computers = []
    unusable_computers = []

    for tr in soup.find_all('tr'):
        computer_name = tr.find('td').text.strip()
        status_color = tr.find('td')['style']

        if 'background-color: red' in status_color:
            unusable_computers.append(computer_name)
        elif 'background-color: yellow' in status_color:
            high_load_computers.append(computer_name)
        elif 'background-color: white' in status_color and computer_name.islower() and computer_name.isalpha():
            usable_computers.append(computer_name)

    return usable_computers, high_load_computers, unusable_computers


if __name__ == "__main__":
    url = "https://apps.cs.utexas.edu/unixlabstatus/"
    usable, high_load, unusable = scrape_website(url)

    print("List of usable computers:")
    print(usable)

    print("\nList of high load computers:")
    print(high_load)

    print("\nList of unusable computers:")
    print(unusable)

    print(len(usable))
