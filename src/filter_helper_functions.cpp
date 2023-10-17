#include "inc/filter_helper_functions.h"

bool substrFilterHandler(SubstringFilter *sf, int *err,
                         std::vector<unsigned char> attribute) {
  std::vector<unsigned char> attributeInital;
  std::vector<unsigned char> attributeMiddle;
  std::vector<unsigned char> attributeFinal;

  // extract initial
  if (!sf->getSubInitial().empty()) {
    if (attribute.size() >= sf->getSubInitial().size()) {
      attributeInital = std::vector<unsigned char>(
          attribute.begin(), attribute.begin() + sf->getSubInitial().size());
      attributeMiddle = std::vector<unsigned char>(
          attribute.begin() + sf->getSubInitial().size(), attribute.end());
    } else {
      return false;
    }
  }
  // extract final
  if (!sf->getSubFinal().empty()) {
    if (attribute.size() >= sf->getSubFinal().size()) {
      attributeFinal = std::vector<unsigned char>(
          attribute.end() - sf->getSubFinal().size(), attribute.end());
      if (attributeMiddle.size() > sf->getSubFinal().size()) {
        attributeMiddle = std::vector<unsigned char>(
            attributeMiddle.begin(),
            attributeMiddle.end() - sf->getSubFinal().size());
      }
    } else {
      return false;
    }
  }

  if (!sf->getSubInitial().empty() && attributeInital != sf->getSubInitial()) {
    return false;
  }
  if (!sf->getSubFinal().empty() && attributeFinal != sf->getSubFinal()) {
    return false;
  }
  unsigned long int x = 0;

  for (unsigned long int y = 0; y < sf->getSubAny().size(); y++) {
    bool match = false;
    for (; x < attributeMiddle.size(); x++) {

      if (attributeMiddle[x] == sf->getSubAny()[y][0]) {
        match = true;
        for (unsigned long int z = 0; z < sf->getSubAny()[y].size(); z++) {
          if (attributeMiddle[x + z] != sf->getSubAny()[y][z]) {
            match = false;
            break;
          }
        }
        if (match) {
          break;
        }
      }
    }
    if (!match) {
      return false;
    }
  }
  return true;
}

bool equalityMatchHandler(equalityMatchFilter *emf, int *err,
                          std::vector<unsigned char> attribute) {
  if (emf->getAssertionValue() == attribute) {
    return true;
  }
  return false;
}

bool filterLine(filter *f, int *err, DatabaseObject &databaseEntry) {

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};

  switch (f->getFilterType()) {
  case equalityMatch: {
    std::vector<unsigned char> attributeDescription =
        ((equalityMatchFilter *)f)->getAttributeDescription();
    if (attributeDescription == cn) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_name());
    } else if (attributeDescription == email) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_email());
    } else if (attributeDescription == uid) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_uid());
    }

  } break;
  case substrings: {
    std::vector<unsigned char> attributeDescription =
        ((SubstringFilter *)f)->getAttributeDescription();
    if (attributeDescription == cn) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_name());
    } else if (attributeDescription == email) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_email());
    } else if (attributeDescription == uid) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_uid());
    }
  } break;
  case AND: {
    andFilter *af = (andFilter *)f;
    for (unsigned long int i = 0; i < af->filters.size(); i++) {
      if (!filterLine(af->filters[i], err, databaseEntry)) {
        return false;
      }
    }
    return true;

  } break;
  case OR: {
    orFilter *of = (orFilter *)f;
    for (unsigned long int i = 0; i < of->filters.size(); i++) {
      if (filterLine(of->filters[i], err, databaseEntry)) {
        return true;
      }
    }
    return false;
  } break;
  case NOT: {
    notFilter *nf = (notFilter *)f;
    return !filterLine((nf->filter), err, databaseEntry);
  } break;
  default:
    return false;
  }
  return false;
}

std::vector<DatabaseObject>
filterHandler(filter *f, int *err, const char *dbLocation, int sizeLimit) {

  std::vector<DatabaseObject> resultDB;
  int dbErr = 0;
  DatabaseController db(dbLocation);
  int lineCounter = 0;
  if (f->getFilterType() == undefined) {
    while (true) {

      DatabaseObject obj = db.loadNextRow(&dbErr);

      if (dbErr != 0)
        break;
      resultDB.push_back(obj);
      lineCounter++;
      if (lineCounter >= sizeLimit && sizeLimit != 0) {
        *err = 1;
        break;
      }
    }
  } else {

    while (true) {

      DatabaseObject obj = db.loadNextRow(&dbErr);

      if (dbErr != 0)
        break;

      if (filterLine(f, err, obj)) {
        resultDB.push_back(obj);
        lineCounter++;
      }

      if (lineCounter >= sizeLimit && sizeLimit != 0) {
        *err = 1;
        break;
      }
    }
  }

  return resultDB;
}

filter *convertToFilterObject(std::vector<unsigned char>::iterator BERfilter) {

  filter *f;
  int err;
  int lenght = 0;
  int ll = 0;
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

  switch (getFilterType(BERfilter)) {
  case equalityMatch:

    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    lenght = ParseLength(BERfilter + 1, &err);

    ll = getLengthLength(BERfilter + 1, &err);

    for (int i = 0; i < lenght; i++) {
      attributeDescription.push_back(BERfilter[1 + ll + i]);
    }

    skipTags(BERfilter, 1, &err);
    if (err != 0)
      return new filter();

    lenght = ParseLength(BERfilter + 1, &err);
    ll = getLengthLength(BERfilter + 1, &err);

    for (int i = 0; i < lenght; i++) {
      assertionValue.push_back(BERfilter[1 + ll + i]);
    }

    f = new equalityMatchFilter(attributeDescription, assertionValue);

    break;
  case substrings: {
    std::vector<unsigned char> attributeDescription;
    std::vector<unsigned char> initial;
    std::vector<std::vector<unsigned char>> any;
    std::vector<unsigned char> final;
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    lenght = ParseLength(BERfilter + 1, &err);
    ll = getLengthLength(BERfilter + 1, &err);

    for (int i = 0; i < lenght; i++) {
      attributeDescription.push_back(BERfilter[1 + ll + i]);
    }

    skipTags(BERfilter, 1, &err);
    if (err != 0)
      return new filter();
    int lenghtOfSequence = ParseLength(BERfilter + 1, &err);
    int lenghtOflenOfSequence = getLengthLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();

    int currentBitPointer = 0;
    while (currentBitPointer < lenghtOfSequence + lenghtOflenOfSequence) {
      switch (BERfilter[0]) {
      case 0x80: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        initial = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                             BERfilter + 1 + LenghtLenght +
                                                 dataLenght);
      } break;
      case 0x81: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        std::vector<unsigned char> tmp = std::vector<unsigned char>(
            BERfilter + 1 + LenghtLenght,
            BERfilter + 1 + LenghtLenght + dataLenght);
        any.push_back(tmp);
      } break;
      case 0x82: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        final = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                           BERfilter + 1 + LenghtLenght +
                                               dataLenght);
      } break;
      }
      currentBitPointer += 1 + getLengthLength(BERfilter + 1, &err) +
                           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }
    f = new SubstringFilter(attributeDescription, initial, any, final);

  } break;
  case AND:
    f = new andFilter();

    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    for (int i = 0; i < lenght;) {

      if (err != 0)
        return new filter();
      filter *tmpF = convertToFilterObject(BERfilter);
      printf("filter type: %d\n", tmpF->getFilterType());
      fflush(stdout);
      ((andFilter *)f)->filters.push_back(tmpF);
      i += 1 + getLengthLength(BERfilter + 1, &err) +
           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }

    break;
  case OR:
    f = new orFilter();
    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();

    for (int i = 0; i < lenght;) {
      if (err != 0)
        return new filter();
      filter *tmpF = convertToFilterObject(BERfilter);
      ((orFilter *)f)->filters.push_back(tmpF);
      i += 1 + getLengthLength(BERfilter + 1, &err) +
           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }

    break;
  case NOT: {
    f = new notFilter();
    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    filter *tmpF = convertToFilterObject(BERfilter);
    ((notFilter *)f)->filter = tmpF;
    break;
  }
  default:
    f = new filter();
    break;
  }
  // print values
  for (unsigned long int i = 0; i < attributeDescription.size(); i++) {
    printf("%c", attributeDescription[i]);
  }
  printf("\n");
  for (unsigned long int i = 0; i < assertionValue.size(); i++) {
    printf("%c", assertionValue[i]);
  }

  return f;
}