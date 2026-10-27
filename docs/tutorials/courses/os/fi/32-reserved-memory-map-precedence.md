# Varatut muistialueet menevät käyttökelpoisten edelle

Copyright (c) 2026 ItsDropper. Lisensoitu MIT-lisenssillä; katso projektin juuren `LICENSE`.

## Tavoite

Tee fyysisen muistin alustuksesta varovainen tilanteessa, jossa firmwarelta saatu Multiboot2-muistikartta sisältää päällekkäisiä alueita. Käyttökelpoiseksi ilmoitettu alue ei saa tehdä sivusta varattavaa, jos sama sivu osuu osittainkin varattuun tai muuten ei-käyttökelpoiseen alueeseen.

Toteutus on tiedostossa `os/mm/phys.vnt`. Se käyttää kahta läpikäyntiä: ensin merkitään käyttökelpoisten (tyyppi 1) alueiden kokonaiset sivut, minkä jälkeen jokainen ei-tyypin-1-alueen koskettama sivu poistetaan käyttökelpoisten bittikartasta. Kernelin ja käynnistystietojen varaukset tehdään tämän jälkeen.

## Miksi kaksi läpikäyntiä?

Muistikartta kuvaa tavallisesti erillisiä alueita, mutta kernelin ei pidä riippua merkintöjen järjestyksestä, jos alueet menevät päällekkäin. Jos käyttökelpoinen merkintä käsitellään varatun merkinnän jälkeen ja se vain asettaa bitin, varattu sivu voi päätyä varaajalle. Kahden läpikäynnin malli antaa ei-käyttökelpoisille alueille etusijan riippumatta taulukon järjestyksestä.

Sivurajat käsitellään tarkoituksella eri tavalla:

- **Käyttökelpoinen alue:** mukaan otetaan vain kokonaiset 4096 tavun sivut. Alku pyöristetään ylöspäin ja loppu alaspäin.
- **Varattu alue:** hylätään jokainen 4096 tavun sivu, jota alue koskettaa. Alku pyöristetään alaspäin ja loppu ylöspäin.

Varovainen sääntö voi hylätä sivun, josta vain osa on käytettävissä, mutta se estää varaajaa palauttamasta kehystä, joka menee päällekkäin firmwarelle kuuluvan muistin kanssa.

## Lähdekoodikartta

- `os/mm/phys.vnt`: `phys_mmap_count()`, `phys_mmap_base()`, `phys_mmap_length()`, `phys_mmap_type()` ja `phys_init()`.
- `os/mm/phys.vnt`: käyttökelpoisten, varattujen ja pysyvästi varattujen kehysten bittikartat.
- `os/kernel/shell.vnt`: `mem`- ja `memtest`-komennot.
- `.github/workflows/vntos-boot.yml`: todelliset build- ja QEMU-käynnistystarkistukset.

## Ensimmäinen läpikäynti: merkitse kokonaiset käyttökelpoiset sivut

Nykyinen käyttökelpoisten alueiden käsittely pyöristää alueen sisäänpäin:

```vnt
start_frame = (base + 4096 - 1) / 4096
end_frame = (base + length) / 4096
if start_frame < 256 {
    start_frame = 256
}
frame = start_frame
while frame < end_frame {
    phys_usable_set(frame, 1)
    frame = frame + 1
}
```

Kehys 256 alkaa yhden mebitavun kohdalta. Yläraja 262144 on ensimmäisen gigatavun ulkopuolinen kehysindeksi. Koodi rajaa jokaisen alueen tuettuun muistialueeseen ennen kehysindeksien laskemista.

## Toinen läpikäynti: poista ei-käyttökelpoisten alueiden koskettamat sivut

```vnt
start_frame = base / 4096
end_frame = (base + length + 4095) / 4096
if end_frame > 262144 {
    end_frame = 262144
}
frame = start_frame
while frame < end_frame {
    phys_usable_set(frame, 0)
    frame = frame + 1
}
```

Koko toteutus tarkistaa ensin, että `base` on tuetun alueen sisällä ja `length` on positiivinen. Sen jälkeen pituus rajataan arvoon `limit - base` ennen loppuosoitteen laskemista. Näin firmwaresta tulevaa rajatonta pituutta ei lisätä perusosoitteeseen.

Alun kokonaislukujako pyöristää alaspäin. Kun loppuun lisätään 4095 ennen jakoa, tulos pyöristyy ylöspäin. Siksi esimerkiksi tavualue 5000–6000 poistaa käyttökelpoisuudesta sen sivun, jonka sisällä tavut sijaitsevat, vaikka alue ei täytä kokonaista sivua.

Kaikki muistialueet, joiden tyyppi ei ole 1, käsitellään tavalliselle varaajalle käyttökelvottomina. Tämä on tarkoituksella varovainen ratkaisu: ACPI:n palautettavaa muistia ja NVS-muistia ei anneta yleiselle sivuvaraajalle ilman erillistä elinkaarikäytäntöä.

## Miksi järjestyksellä on väliä?

Oletetaan, että kartassa on seuraavat merkinnät:

- käyttökelpoinen: `0x200000–0x600000`
- varattu: `0x300800–0x301000`

Varattu alue koskettaa kehystä `0x300000 / 4096`. Ensimmäinen läpikäynti voi merkitä kehyksen käyttökelpoiseksi, koska suurempi tyypin 1 alue sisältää sen. Toinen läpikäynti poistaa saman kehyksen käyttökelpoisten bittikartasta. Merkintöjen järjestyksen kääntäminen ei muuta lopputulosta.

Molempien läpikäyntien jälkeen `phys_init()` varaa matalan muistin ja kernelin alueen sekä Multiboot2-tietolohkon. Sen jälkeen käyttökelpoisten ja vapaiden kehysten laskurit lasketaan lopullisesta bittikartasta.

## Rajat ja virhetilanteet

1. **Alue alkaa vähintään 1 GiB:n kohdalla:** ohita se, koska tämä varaaja ei tue sitä.
2. **Alue ylittää 1 GiB:n rajan:** rajaa pituus ennen loppuosoitteen laskemista.
3. **Pituus on nolla tai negatiivinen:** ohita merkintä.
4. **Käyttökelpoinen alue alkaa kesken sivun:** osittainen ensimmäinen sivu ei ole käyttökelpoinen.
5. **Varattu alue loppuu kesken sivun:** viimeinen kosketettu sivu poistetaan käyttökelpoisuudesta.
6. **Varattu merkintä menee päällekkäin käyttökelpoisen kanssa:** toinen läpikäynti voittaa.
7. **Varattu merkintä on taulukossa ennen käyttökelpoista:** tulos pysyy samana, koska kaikki varatut alueet käsitellään toisessa läpikäynnissä.
8. **Kernel sijaitsee käyttökelpoisella alueella:** myöhempi kernelin varaus merkitsee sen kehykset varatuiksi.

## Varmistus

Suorita projektin juuressa kernelin normaali build ja käynnistä se QEMUssa:

```sh
bash os/build.sh
qemu-system-x86_64 -cdrom build/vntos.iso -m 128M -serial stdio
```

Suorita vierasjärjestelmän komentotulkissa `mem` ja `memtest`. Nykyiset testit tarkistavat varaus- ja vapautuslaskurit sekä sen, ettei kernelin varattua muistia voi vapauttaa. Ne eivät luo keinotekoista päällekkäistä Multiboot2-muistikarttaa, joten älä väitä, että erillinen päällekkäisyystesti olisi jo olemassa. Tarkista kyseisen commitin GitHub Actions -tulos.

Hyödyllinen tuleva testi syöttäisi synteettisen luettelon muistialueita puhtaalle alueiden normalisointifunktiolle ja varmistaisi, että lopullinen bittikartta ei riipu merkintöjen järjestyksestä. Testi ei saa muuttaa käynnissä olevan varaajan bittikarttoja.

## Toteutusharjoitus

1. Selitä, miksi käyttökelpoiset alueet pyöristetään sisäänpäin ja varatut ulospäin.
2. Laske kehysindeksit alueelle, joka alkaa tavusta 5000 ja päättyy tavuun 6000.
3. Selitä, miksi pituus on rajattava ennen laskua `base + length + 4095`.
4. Seuraa, miten lopullisesta bittikartasta lasketaan `state_get(28)` ja `state_get(29)`.
5. Suunnittele synteettinen päällekkäisyystesti, jossa käyttökelpoisten ja varattujen merkintöjen järjestys vaihdetaan.
6. Älä merkitse ACPI:n palautettavaa muistia käyttökelpoiseksi ennen kuin kernelissä on erillinen palautuskäytäntö.

## Hyväksymiskriteerit

- [ ] Käyttökelpoiset merkinnät merkitsevät vain kokonaisia sivuja.
- [ ] Muut kuin tyypin 1 merkinnät poistavat kaikki kosketetut sivut käyttökelpoisuudesta.
- [ ] Varatut merkinnät voittavat riippumatta muistikartan järjestyksestä.
- [ ] Alueen rajat tarkistetaan ennen loppuosoitteen laskemista.
- [ ] Kernelin ja Multiboot2-tietojen varaukset tehdään edelleen molempien läpikäyntien jälkeen.
- [ ] Olemassa olevat `memtest`-, käynnistys- ja komentotulkkitestit läpäisevät todellisten CI/QEMU-tulosten mukaan.
- [ ] Molemmat kurssien sisällysluettelot ja edellinen oppitunti linkittävät tähän.

## Nykyiset rajoitukset

Muutos tekee bittikartan muodostamisesta varovaisemman. Se ei lisää hotplug-tukea, palauta ACPI-muistia käyttöön, tue yli 1 GiB:n fyysisiä osoitteita eikä tee varaajasta SMP-turvallista. Varaaja käyttää edelleen käynnistyksen bittikarttoja ja edellyttää sarjoitettua yhden CPU:n käyttöä.

---

[← Edellinen oppitunti](./31-contiguous-physical-pages.md) · [Sisällysluettelo](./README.md) · **Kurssi suoritettu.**
