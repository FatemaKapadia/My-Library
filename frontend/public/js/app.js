const API_URL = 'http://localhost:8080/api';

// State
let allBooks = [];
let recommendations = [];

// DOM Elements
const views = document.querySelectorAll('.view');
const navLinks = document.querySelectorAll('.nav-links li');
const modal = document.getElementById('book-modal');
const closeBtn = document.querySelector('.close-btn');
const addBookBtn = document.getElementById('add-book-btn');
const form = document.getElementById('book-form');

// Grids
const libraryGrid = document.getElementById('library-grid');
const wishlistGrid = document.getElementById('wishlist-grid');
const recGrid = document.getElementById('recommendation-grid');
const ledgerGrid = document.getElementById('ledger-grid');

// Stats
const statTotal = document.getElementById('stat-total');
const statToRead = document.getElementById('stat-toread');
const statLent = document.getElementById('stat-lent');

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    fetchBooks();
    
    // Navigation
    navLinks.forEach(link => {
        link.addEventListener('click', (e) => {
            navLinks.forEach(l => l.classList.remove('active'));
            views.forEach(v => v.classList.remove('active'));

            const targetView = e.target.getAttribute('data-view');
            e.target.classList.add('active');
            document.getElementById(`${targetView}-view`).classList.add('active');

            if(targetView === 'wishlist') {
                fetchRecommendations();
            }
        });
    });

    // Modal Control
    addBookBtn.addEventListener('click', () => {
        form.reset();
        document.getElementById('book-id').value = '';
        document.getElementById('modal-title').innerText = 'Add a Book';
        modal.classList.add('visible');
    });

    closeBtn.addEventListener('click', () => {
        modal.classList.remove('visible');
    });

    // Handle Form Submit
    form.addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const btn = form.querySelector('.submit-btn');
        btn.innerText = 'Saving & Fetching info...';
        btn.disabled = true;

        const id = document.getElementById('book-id').value;
        const bookData = {
            title: document.getElementById('book-title').value,
            author: document.getElementById('book-author').value,
            genre: document.getElementById('book-genre').value,
            status: document.getElementById('book-status').value,
            rating: parseInt(document.getElementById('book-rating').value) || 0,
            lent_status: document.getElementById('book-lent-status').value,
            person_name: document.getElementById('book-person').value,
            date: document.getElementById('book-date').value,
            notes: document.getElementById('book-notes').value
        };

        try {
            if (id) {
                await fetch(`${API_URL}/books/${id}`, {
                    method: 'PUT',
                    body: JSON.stringify(bookData)
                });
            } else {
                await fetch(`${API_URL}/books`, {
                    method: 'POST',
                    body: JSON.stringify(bookData)
                });
            }
            modal.classList.remove('visible');
            await fetchBooks();
        } catch (error) {
            console.error("Error saving book:", error);
            alert("Failed to save book");
        } finally {
            btn.innerText = 'Save Book';
            btn.disabled = false;
        }
    });
});

async function fetchBooks() {
    try {
        const res = await fetch(`${API_URL}/books`);
        allBooks = await res.json();
        updateUI();
    } catch(err) {
        console.error("Server not reachable", err);
    }
}

async function fetchRecommendations() {
    try {
        const res = await fetch(`${API_URL}/recommendations`);
        recommendations = await res.json();
        renderGrid(recGrid, recommendations, true);
    } catch(err) {
        console.error(err);
    }
}

function updateUI() {
    // Stats
    statTotal.innerText = allBooks.filter(b => b.status === 'Owned').length;
    statToRead.innerText = allBooks.filter(b => b.status === 'ToRead').length;
    statLent.innerText = allBooks.filter(b => b.lent_status !== 'None').length;

    // Filters
    const owned = allBooks.filter(b => b.status === 'Owned');
    const wishlist = allBooks.filter(b => b.status === 'ToBuy' || b.status === 'ToRead');
    const ledger = allBooks.filter(b => b.lent_status !== 'None');

    renderGrid(libraryGrid, owned);
    renderGrid(wishlistGrid, wishlist);
    renderGrid(ledgerGrid, ledger);
}

function renderGrid(container, books, isRec = false) {
    container.innerHTML = '';
    books.forEach(b => {
        const card = document.createElement('div');
        card.className = 'book-card';
        
        let placeholderSVG = `data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" width="300" height="450" viewBox="0 0 300 450"><rect width="300" height="450" fill="%231e293b"/><text x="50%25" y="50%25" fill="%2394a3b8" font-family="sans-serif" font-size="24" text-anchor="middle" dy=".3em">No Cover</text></svg>`;
        let cover = b.cover_url || placeholderSVG;
        if (cover.startsWith('http://')) {
            cover = cover.replace('http://', 'https://');
        }
        
        // Stars
        let stars = isRec ? b.global_rating : b.rating;
        let badgeText = isRec ? `⭐ ${stars} (Global)` : `⭐ ${stars}`;
        
        // Ledger extra info
        let ledgerHtml = '';
        if (b.lent_status !== 'None') {
            ledgerHtml = `<div style="color: #fca5a5; font-size: 0.9rem; margin-top: 0.5rem;">
                ${b.lent_status === 'LentOut' ? 'Lent to' : 'Borrowed from'}: <strong>${b.person_name}</strong>
                <br><small>${b.date}</small>
            </div>`;
        }

        card.innerHTML = `
            ${stars > 0 ? `<div class="book-badge">${badgeText}</div>` : ''}
            <img src="${cover}" class="book-cover" alt="Cover">
            <div class="book-title">${b.title}</div>
            <div class="book-author">${b.author}</div>
            <div class="book-genre">${b.genre || 'Uncategorized'}</div>
            ${ledgerHtml}
        `;
        
        // Edit functionality (not on rec grid)
        if(!isRec) {
            const editBtn = document.createElement('button');
            editBtn.innerText = 'Edit';
            editBtn.style = 'margin-top: 10px; background: rgba(255,255,255,0.1); border:none; color: white; padding: 5px; cursor: pointer; border-radius: 4px;';
            editBtn.onclick = () => openEditModal(b);
            card.appendChild(editBtn);
        }

        container.appendChild(card);
    });
}

function openEditModal(b) {
    document.getElementById('modal-title').innerText = 'Edit Book';
    document.getElementById('book-id').value = b.id;
    document.getElementById('book-title').value = b.title;
    document.getElementById('book-author').value = b.author;
    document.getElementById('book-genre').value = b.genre;
    document.getElementById('book-status').value = b.status;
    document.getElementById('book-rating').value = b.rating;
    document.getElementById('book-lent-status').value = b.lent_status;
    document.getElementById('book-person').value = b.person_name || '';
    document.getElementById('book-date').value = b.date || '';
    document.getElementById('book-notes').value = b.notes || '';
    
    modal.classList.add('visible');
}
