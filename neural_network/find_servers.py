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
    
    # delete the servers file servers.txt and create a new one
    with open('servers.txt', 'w') as f:
        f.write('')
    
    # write the usable servers to the servers.txt file and write usuable
    # firs print "USUABLE"
    with open('servers.txt', 'a') as f:
        f.write('USUABLE\n')
        for server in usable:
            f.write(server + '\n')
    
    # write the high load servers to the servers.txt file and write high load
    # first print "HIGH LOAD"
    with open('servers.txt', 'a') as f:
        f.write('HIGH LOAD\n')
        for server in high_load:
            f.write(server + '\n')
    
    # write the unusable servers to the servers.txt file and write unusable
    # first print "UNUSABLE"
    with open('servers.txt', 'a') as f:
        f.write('UNUSABLE\n')
        for server in unusable:
            f.write(server + '\n')
        