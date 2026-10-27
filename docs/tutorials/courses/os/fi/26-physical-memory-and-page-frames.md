# Fyysinen muisti ja sivukehykset

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Fyysisen muistin varaaja antaa kernelille RAM-alueita. Tavallinen tapa on jakaa muisti 4096 tavun sivukehyksiin ja pitää kirjaa vapaista kehyksistä. Varaaja ei saa antaa käyttöön jo varattua kehystä tai firmwarelle kuuluvaa muistia.

## Sanasto

Sivukehys on fyysinen muistialue. Bittikartta tallentaa kunkin kehyksen tilan biteillä. Varaus merkitsee kehyksen käytetyksi, vapautus tekee sen taas saatavaksi. Invariantti on sääntö, jonka on aina pysyttävä totena.

## Käytännön harjoitus

Avaa os/mm/phys.vnt. Etsi alustustoiminto, varaus, vapautus, bittikartta ja vapaiden kehysten laskuri. Selvitä, miten käytettävät muistialueet valitaan ja mitä tapahtuu, kun vapaa muisti loppuu.

## Yleinen virhe ja turvallisuus

Varaaja ei ole yleisesti turvallinen usean CPU:n samanaikaisessa käytössä. Kaksi CPU:ta voi lukea saman vapaan bitin ennen päivitystä. Tarvitaan synkronointi ja moniydintestit.

## Tarkista ymmärtäminen

Miksi samaa kehystä ei saa antaa kahdelle kutsujalle? Miksi bittikartan ja laskurin muutokset on suojattava yhdessä?

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Rakenna ja testaa keskeytyskernel](24-build-and-test-the-interrupt-kernel.md) · [Seuraava oppitunti: Loppuprojekti: tee todennettava parannus →](25-graduation-project.md)
